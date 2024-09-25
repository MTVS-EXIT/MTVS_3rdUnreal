// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DronePlayer.h"
#include "KHS/KHS_DroneMainUI.h"
#include "KHS/KHS_AIVisionObject.h"
#include <KHS/KHS_JsonParseLib.h>

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"
#include "Components/AudioComponent.h"

#include "Engine/StaticMesh.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "TextureResource.h"
#include "Misc/FileHelper.h"
#include "Net/VoiceConfig.h"

#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/VoiceInterface.h"

#include "HttpModule.h"
#include "HttpFwd.h"

#include "Kismet/GameplayStatics.h"
#include "KJH/KJH_PlayerState.h"
#include <KJH/KJH_PlayerController.h>




// Sets default values
AKHS_DronePlayer::AKHS_DronePlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//충돌체, 메시, 카메라
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->SetRelativeLocation(FVector(0, -3, 20));
	CameraComp->bUsePawnControlRotation = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/Mesh/Drone01/drone_costum.drone_costum'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(tempMesh.Object);
		MeshComp->SetRelativeLocationAndRotation(FVector(-10, -2, -10), FRotator(0, -90, 0));
		MeshComp->SetRelativeScale3D(FVector(0.15f));
	}

	// VOIP Talker 컴포넌트를 생성하고, VOIPTalkerComponent 포인터에 할당.
	VOIPTalkerComp = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalkerComp"));


	// 이미지 캡쳐 용도 렌더 타겟 생성 및 설정
	RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	RenderTarget->InitAutoFormat(1280, 720); // 원하는 해상도로 초기화
	
	//카메라쉐이크 스탯
	DroneShakeInterval = 0.5f;  //0.5초마다 카메라쉐이크
	TimeSinceLastShake = 0.0f;

	//Hovering 스탯
	HoverAmplitude = 3.0f;
	HoverFrequency = 1.0f;
	RollAmplitude = 3.0f;
	RollFrequency = 1.0f;

	//Drone 스탯 초기화
	//DroneMaxSpeed = 3000.0f;
	DroneMaxSpeed = 1500.0f;
	//DroneAccelerateRate = 450.0f;
	DroneAccelerateRate = 300.0f;
	//DroneDecelerateRate = 400.0f;
	DroneDecelerateRate = 250.0f;
	DroneCurrentSpeed = FVector::ZeroVector;
	DroneAcceleration = FVector::ZeroVector;

	//RPC설정 추가
	SetReplicates(true);
	SetReplicateMovement(true);

	//SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	//SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//SphereComp->SetSimulatePhysics(true);  // 물리 시뮬레이션 활성화
	//SphereComp->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
}

// Called when the game starts or when spawned
void AKHS_DronePlayer::BeginPlay()
{
	Super::BeginPlay();
	

	//IMC_TPS
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(IMC_Drone, 0);
		}
	}
	
	////드론 Main UI초기화
	////로컬 플레이어인 경우에만 UI생성하도록 수정
	//if (IsLocallyControlled() && !DroneMainUI)
	//{
	//	if (DroneMainUIFactory)
	//	{
	//		DroneMainUI = CreateWidget<UUserWidget>(GetWorld(), DroneMainUIFactory);
	//		if(DroneMainUI)
	//		{
	//			DroneMainUI->AddToViewport(0);
	//			UE_LOG(LogTemp, Warning, TEXT("Drone UI created for local player"));
	//		}
	//	}
	//}


	//메시 위치, 회전 저장
	OriginalMeshLocation = MeshComp->GetRelativeLocation();
	OriginalMeshRotation = MeshComp->GetRelativeRotation();

	// VOIP 초기화 작업 호출
	InitializeVOIP();

	// 월드에서 SceneCapture2D 액터를 가져옴
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASceneCapture2D::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		SceneCaptureActor = Cast<ASceneCapture2D>(FoundActors[0]);
	}
	if (SceneCaptureActor)
	{
		USceneCaptureComponent2D* SceneCaptureComponent = SceneCaptureActor->GetCaptureComponent2D();
		if (SceneCaptureComponent && CameraComp)
		{
			// CameraComp에 SceneCaptureComponent를 Attach
			SceneCaptureComponent->AttachToComponent(CameraComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SceneCaptureComponent->TextureTarget = RenderTarget;
			SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
			SceneCaptureComponent->FOVAngle = CameraComp->FieldOfView; // 드론 카메라와 같은 FOV
		}
		// 초기 SceneCaptureActor의 설정을 드론 카메라와 동기화
		SyncSceneCaptureWithCamera();
	}

	// 드론 위치에서 반복 재생되는 사운드 설정
	if (FlightSFXFactory)
	{
		// 오디오 컴포넌트 생성 및 설정
		FlightAudioComponent = NewObject<UAudioComponent>(this);
		if (FlightAudioComponent)
		{
			FlightAudioComponent->SetSound(FlightSFXFactory);  // 사운드 설정
			FlightAudioComponent->SetWorldLocation(GetActorLocation());  // 드론의 위치
			FlightAudioComponent->bAutoActivate = false;  // 수동으로 재생 시작
			FlightAudioComponent->bIsUISound = false;  // 3D 사운드로 설정
			FlightAudioComponent->bAllowSpatialization = true;  // 공간 사운드 활성화
			FlightAudioComponent->AttenuationSettings = FlightSoundAttenuation;  // 감쇠 설정
			//FlightAudioComponent->bLooping = true;  // 반복 재생 설정

			// 오디오 컴포넌트를 드론의 메시에 Attach (메시 컴포넌트에 붙임)
			FlightAudioComponent->SetupAttachment(SphereComp);
			FlightAudioComponent->RegisterComponent();  // 컴포넌트를 등록

			// 사운드 재생 시작
			FlightAudioComponent->Play();
		}
	}
}

// Called every frame
void AKHS_DronePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//로컬 플레이어가 아니면 작동 안함
	if (false == IsLocallyControlled())
	{
		return;
	}

	// Drone 이동 처리
	FVector NewVelocity = DroneCurrentSpeed + (DroneAcceleration * DeltaTime);

	for (int32 i = 0; i < 3; i++)
	{
		// 반대 방향의 가속도가 들어올 때 서서히 감속
		if (FMath::Sign(DroneAcceleration[i]) != FMath::Sign(DroneCurrentSpeed[i]) && !FMath::IsNearlyZero(DroneCurrentSpeed[i]))
		{
			// 현재 속도를 서서히 0으로 감소시킴
			DroneCurrentSpeed[i] = FMath::FInterpTo(DroneCurrentSpeed[i], 0.0f, DeltaTime, DroneDecelerateRate);
		}
		else
		{
			// 가속도가 같은 방향이면 일반 가속도 적용
			DroneCurrentSpeed[i] = NewVelocity[i];
		}

		// 키 입력이 없을 때 서서히 멈추도록 처리
		if (DroneAcceleration.IsNearlyZero())
		{
			DroneCurrentSpeed[i] = FMath::FInterpTo(DroneCurrentSpeed[i], 0.0f, DeltaTime, DroneDecelerateRate);
		}
	}

	// 최대 속도 제한
	DroneCurrentSpeed = DroneCurrentSpeed.GetClampedToMaxSize(DroneMaxSpeed);

	// 위치 업데이트
	if (!DroneCurrentSpeed.IsNearlyZero())
	{
		FVector NewLocation = GetActorLocation() + (DroneCurrentSpeed * DeltaTime);
		SetActorLocation(NewLocation);
	}

	// 프레임이 끝날 때 가속도 초기화
	DroneAcceleration = FVector::ZeroVector;

	//고도계 업데이트 함수
	DroneAltitudeUpdate();

	//사물 윤곽선 표시 함수
	DroneShowOutline();
	
	//카메라쉐이크 타이머 업데이트
	TimeSinceLastShake += DeltaTime;
	if (TimeSinceLastShake >= DroneShakeInterval)
	{
		PlayDroneCameraShake();
		TimeSinceLastShake = 0.0f; //타이머 초기화
	}

	// 상하 Hovering 효과 적용
	float HoverOffset = HoverAmplitude * FMath::Sin(GetWorld()->TimeSeconds * HoverFrequency);
	FVector NewLocation = OriginalMeshLocation + FVector(0, 0, HoverOffset);
	MeshComp->SetRelativeLocation(NewLocation);

	// 좌우 파도 효과 적용
	float RollOffset = RollAmplitude * FMath::Sin(GetWorld()->TimeSeconds * RollFrequency);
	float PitchOffset = RollAmplitude * FMath::Cos(GetWorld()->TimeSeconds * RollFrequency);
	FRotator NewRotation = OriginalMeshRotation + FRotator(PitchOffset, 0, RollOffset);
	MeshComp->SetRelativeRotation(NewRotation);

	// 시야에 있는 모든 태그를 가진 Actor를 감지하고 위젯 컴포넌트를 활성화/비활성화
	// 이제는 시야가 아니라 OnResGetAIImage에서 관리
	//TArray<FString> TagsToCheck = { TEXT("Safe"), TEXT("Caution"), TEXT("Danger") };
	//CheckVisionForTags(TagsToCheck);


	//Post Process(Radial Blur, Depth of Field) 설정 함수
	SetDronePostProcess();

	// 매 프레임마다 SceneCaptureActor를 드론 카메라와 동기화
	SyncSceneCaptureWithCamera();

}

// Called to bind functionality to input
void AKHS_DronePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// DronePlayer에 대해 subsys 만들기(Client인 경우 필수필요)
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(IMC_Drone, 0);
			UE_LOG(LogTemp, Warning, TEXT("Added IMC_Drone mapping context"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get EnhancedInputLocalPlayerSubsystem"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is Null"));
		return;
	}

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_DroneLook, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneLook);
		input->BindAction(IA_DroneFwd, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveFwd);
		input->BindAction(IA_DroneRight, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveRight);
		input->BindAction(IA_DroneUp, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveUp);
		input->BindAction(IA_DroneDown, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveDown);
		input->BindAction(IA_Function, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::SaveCaptureToImage);
		input->BindAction(IA_Voice, ETriggerEvent::Started, this, &AKHS_DronePlayer::StartVoiceChat);
		input->BindAction(IA_Voice, ETriggerEvent::Completed, this, &AKHS_DronePlayer::CancelVoiceChat);
	}
}

//재시작시 Drone Player Possess를 다시 잡아줌
void AKHS_DronePlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); // 기본 클래스의 Possess 로직을 다시 실행하여 Pawn과 Controller 간의 연결을 설정

	// 입력 컴포넌트 초기화
	// Possess 이후에 입력 컴포넌트를 다시 설정하여 드론의 입력 바인딩이 올바르게 이루어지도록 함.
	SetupPlayerInputComponent(NewController->InputComponent);

	// VOIP 관련 초기화 작업
	// 드론이 새로 소유되었을 때 필요한 VOIP 설정 등을 여기서 수행 (보이스채팅 관련)
	InitializeVOIP();  // VOIP Talker의 초기화 및 등록 작업을 처리

	// 필요 시 다른 초기화 작업 추가
	// 예: 드론 카메라 세팅, UI 초기화, 기타 네트워크 설정 등

	// Enhanced Input 시스템에 드론의 입력 매핑 추가
	APlayerController* pc = Cast<APlayerController>(NewController);
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(IMC_Drone, 0); // 입력 매핑 추가
		}
	}

}

#pragma region Drone Camera Effect
//카메라쉐이크 재생함수
void AKHS_DronePlayer::PlayDroneCameraShake()
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && DroneCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DroneCameraShake);
	}
}
//고도계 업데이트 함수
void AKHS_DronePlayer::DroneAltitudeUpdate()
{
	// 고도계, 사물 윤곽선 표시 라인트레이스
	FVector s = GetActorLocation();
	FVector e = s - FVector(0, 0, 10000); //아래로 N만큼 라인트레이스 발사
	FHitResult HitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, s, e, ECC_Visibility, params);

	if (bHit)
	{
		//Drone 고도계 업데이트
		float Altitude = s.Z - HitResult.Location.Z;

		if (DroneMainUI)
		{
			UTextBlock* HeightText = Cast<UTextBlock>(DroneMainUI->GetWidgetFromName(TEXT("HeightText")));
			if (HeightText)
			{
				HeightText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), Altitude)));
			}
		}
	}
}
//Post Process(Radial Blur, Depth of Field) 설정 함수
void AKHS_DronePlayer::SetDronePostProcess()
{
	// 라인트레이스 및 초점 거리 계산
	FVector StartLocation = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 10000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

	if (bHit)
	{
		FocusDistance = FVector::Dist(StartLocation, HitResult.Location);
	}
	else
	{
		FocusDistance = 10000.0f;
	}

	CameraComp->PostProcessSettings.WeightedBlendables.Array.Empty();

	// Radial Blur Material이 카메라에 적용된 상태인지 확인
	if (RadialBlurMaterial && MPC_DroneBlur)
	{
		// 드론의 속도에 따라 Radial Blur 설정
		float CurrentSpeed = DroneCurrentSpeed.Size();
		float SpeedBlurAmount = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, DroneMaxSpeed), FVector2D(0.12f, 0.25f), CurrentSpeed);

		// Material Parameter Collection에 속도 값을 전달
		UMaterialParameterCollectionInstance* MaterialParamInstance = GetWorld()->GetParameterCollectionInstance(MPC_DroneBlur);
		if (MaterialParamInstance)
		{
			MaterialParamInstance->SetScalarParameterValue(FName("BlurAmount"), SpeedBlurAmount);
		}

		// Radial Blur을 포스트 프로세스 설정에 반영 
		PostProcessSettings.bOverride_BloomIntensity = true;
		PostProcessSettings.BloomIntensity = 1;//SpeedBlurAmount;

		// 포스트 프로세스 설정 업데이트(Depth of Field)
		PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
		PostProcessSettings.DepthOfFieldFocalDistance = FocusDistance;

		// Aperture (F-Stop) 값을 설정
		PostProcessSettings.bOverride_DepthOfFieldFstop = true;
		PostProcessSettings.DepthOfFieldFstop = 1.0f;  // F-Stop 값 설정

		// Maximum Aperture (Min F-Stop) 값을 설정
		PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
		PostProcessSettings.DepthOfFieldMinFstop = 11.0f;  // Min Aperture 값 설정

		// 드론의 카메라에 포스트 프로세스 설정 적용
		CameraComp->PostProcessSettings = PostProcessSettings;

		FWeightedBlendable Blendable;
		Blendable.Object = RadialBlurMaterial;
		Blendable.Weight = 1.0f; // 필요에 따라 조정 가능
		CameraComp->PostProcessSettings.WeightedBlendables.Array.Add(Blendable);

		//속도값 출력
		GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, FString::Printf(TEXT("Speed : %f"), CurrentSpeed));
	}
}
//Drone Outline PostProcess 효과 함수
void AKHS_DronePlayer::DroneShowOutline()
{
	// 라인트레이스 범위 내에서 감지된 새로운 액터를 추적할 임시 변수
	TSet<AKHS_AIVisionObject*> DetectedOutlineObjects;

	// 카메라 위치와 방향
	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();

	// 카메라의 시야각(FOV)을 기준으로 여러 방향으로 라인트레이스 발사
	float FOV = CameraComp->FieldOfView; // 카메라의 시야각
	int32 NumRays = 30; // 발사할 라인트레이스의 개수
	float MaxDistance = 10000.0f; // 라인트레이스의 최대 거리


	for (int32 i = 0; i < NumRays; i++)
	{
		// 시야각 범위 내에서 라인트레이스 방향 계산
		float Angle = FMath::Lerp(-FOV / 2, FOV / 2, (float)i / (float)(NumRays - 1));
		FRotator Rotator = FRotator(0, Angle, 0);
		FVector Direction = Rotator.RotateVector(ForwardVector);
		FVector End = Start + (Direction * MaxDistance);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);  // 자신을 무시

		// 라인트레이스 실행
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor && HitActor->IsA(AKHS_AIVisionObject::StaticClass()))
			{
				AKHS_AIVisionObject* HitAIVisionObject = Cast<AKHS_AIVisionObject>(HitActor);

				// 현재 감지된 액터를 임시 변수에 추가
				DetectedOutlineObjects.Add(HitAIVisionObject);

				// 맞은 액터가 KHS_AIVisionObject일 경우 윤곽선 적용
				DroneEnableOutline(HitActor);
			}
			//else
			//{
			//	// 윤곽선 효과 해제
			//	DroneDisableOutline(HitActor);
			//}

		}
	}

	// 이전에 감지된 액터 중에서 이번 라인트레이스에서 감지되지 않은 액터를 찾아 윤곽선 해제
	for (AKHS_AIVisionObject* DetectedObject : DetectedAIVisionObjects)
	{
		if (!DetectedOutlineObjects.Contains(DetectedObject))
		{
			// 윤곽선 효과 해제
			DroneDisableOutline(DetectedObject);
		}
	}

	// 현재 감지된 액터들로 추적 세트 업데이트
	DetectedAIVisionObjects = DetectedOutlineObjects;
}
// 라인트레이스 기반 윤곽선 표시 함수
void AKHS_DronePlayer::DroneEnableOutline(AActor* HitActor)
{
	if (HitActor && HitActor->IsA(AKHS_AIVisionObject::StaticClass())) // Actor가 KHS_AIVisionObject 타입인지 확인
	{
		UStaticMeshComponent* HitMeshComp = Cast<UStaticMeshComponent>(HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (HitMeshComp)
		{
			HitMeshComp->SetRenderCustomDepth(true);
			HitMeshComp->CustomDepthStencilValue = 1; // 스텐실 값을 1로 설정
		}
	}
}
// 라인트레이스 기반 시야 벗어났을때 윤곽선 해제 함수
void AKHS_DronePlayer::DroneDisableOutline(AActor* HitActor)
{
	if (HitActor && HitActor->IsA(AKHS_AIVisionObject::StaticClass()))  // Actor가 KHS_AIVisionObject 타입인지 확인
	{
		UStaticMeshComponent* HitMeshComp = Cast<UStaticMeshComponent>(HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (HitMeshComp)
		{
			// CustomDepth를 비활성화하여 윤곽선 효과 해제
			HitMeshComp->SetRenderCustomDepth(false);
		}
	}
}
#pragma endregion

#pragma region Drone Move Settings

void AKHS_DronePlayer::DroneLook(const FInputActionValue& Value)
{
	// 입력된 2D 벡터 값을 가져와 카메라의 Yaw(X) 와 Pitch(Y) 를 조정
	FVector2D v = Value.Get<FVector2D>();
	// Yaw(좌우 회전) 입력 처리
	AddControllerYawInput(v.X);
	// Pitch(상하 회전) 입력 처리
	AddControllerPitchInput(-v.Y);
}

void AKHS_DronePlayer::DroneMoveFwd(const FInputActionValue& Value)
{
	float ForwardValue = Value.Get<float>();

	// 카메라의 방향을 기반으로 이동 벡터를 계산
	FRotator ControlRotation = GetControlRotation();
	FRotator YawRotation(0, ControlRotation.Yaw, 0); // Pitch 제거하여 Yaw만 사용
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);

	// 드론 가속도에 카메라의 앞 방향을 기준으로 적용
	DroneAcceleration += ForwardDirection * ForwardValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveRight(const FInputActionValue& Value)
{
	float RightValue = Value.Get<float>();

	// 카메라의 방향을 기반으로 이동 벡터를 계산
	FRotator ControlRotation = GetControlRotation();
	FRotator YawRotation(0, ControlRotation.Yaw, 0); // Pitch 제거하여 Yaw만 사용
	FVector RightDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	// 드론 가속도에 카메라의 오른쪽 방향을 기준으로 적용
	DroneAcceleration += RightDirection * RightValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveUp(const FInputActionValue& Value)
{
	float UpValue = Value.Get<float>();
	DroneAcceleration.Z += UpValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveDown(const FInputActionValue& Value)
{
	float DownValue = Value.Get<float>();
	DroneAcceleration.Z -= DownValue * DroneAccelerateRate;
}

#pragma endregion

#pragma region VOIP Talker Setting

//마이크 임계값 설정
void AKHS_DronePlayer::SetMicThreshold(float Threshold)
{
	if (VOIPTalkerComp)
	{
		UVOIPStatics::SetMicThreshold(Threshold);
	}
}
//플레이어 State 등록
void AKHS_DronePlayer::RegisterWithPlayerState()
{
	if (VOIPTalkerComp && GetPlayerState())
	{
		VOIPTalkerComp->RegisterWithPlayerState(GetPlayerState());
	}
}
//로컬 플레이어가 제어중인지 체크
bool AKHS_DronePlayer::IsLocallyControlled() const
{
	return IsPlayerControlled();
}
//StartNetworkVoice 네트워크로 사운드를 보냄
void AKHS_DronePlayer::SetUpNetworkVoice()
{
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = GetController<APlayerController>();
		if (PlayerController)
		{
			IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
			if (OnlineSub)
			{
				IOnlineVoicePtr VoiceInterface = OnlineSub->GetVoiceInterface();
				if (VoiceInterface.IsValid())
				{
					// 플레이어에 Voice Channel 할당
					VoiceInterface->StartNetworkedVoice(PlayerController->GetLocalPlayer()->GetControllerId());
				}
			}
		}
	}
}
//StopNetworkVoice 네트워크로 사운드 보내기 중지
void AKHS_DronePlayer::StopVoice()
{
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = GetController<APlayerController>();
		if (PlayerController)
		{
			IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
			if (OnlineSub)
			{
				IOnlineVoicePtr VoiceInterface = OnlineSub->GetVoiceInterface();
				if (VoiceInterface.IsValid())
				{
					VoiceInterface->StopNetworkedVoice(PlayerController->GetLocalPlayer()->GetControllerId());
				}
			}
		}
	}
}
//VOIP 대상자 등록
void AKHS_DronePlayer::RegisterRemoteTalker()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineVoicePtr VoiceInterface = OnlineSub->GetVoiceInterface();
			if (VoiceInterface.IsValid())
			{
				// 플레이어의 고유 네트워크 ID를 가져옵니다.
				TSharedPtr<const FUniqueNetId> UniqueNetId = PlayerController->PlayerState->UniqueId.GetUniqueNetId();

				if (UniqueNetId.IsValid())
				{
					// 고유 네트워크 ID를 사용하여 원격 Talker를 등록합니다.
					VoiceInterface->RegisterRemoteTalker(*UniqueNetId);
				}
			}
		}
	}
}
//VOIP관련 초기화 작업
void AKHS_DronePlayer::InitializeVOIP()
{
	if (VOIPTalkerComp)
	{
		// VOIPTalkerComponent가 유효한지 확인
		if (IsValid(VOIPTalkerComp))
		{
			// 플레이어 상태에 VOIPTalker를 등록
			RegisterWithPlayerState();

			// 마이크 임계값을 설정
			SetMicThreshold(-1.0f);

			// 로컬 플레이어가 제어 중일 때만 VOIP 관련 설정을 진행
			if (IsLocallyControlled())
			{
				// 콘솔 명령을 실행하여 VOIP를 활성화
				APlayerController* PlayerController = Cast<APlayerController>(GetController());
				if (PlayerController)
				{
					PlayerController->ConsoleCommand("OSS.VoiceLoopback 1");
				}
			}
			// 원격 Talker 등록
			RegisterRemoteTalker();

		}
	}
}

void AKHS_DronePlayer::StartVoiceChat(const FInputActionValue& Value)
{
	GetController<AKJH_PlayerController>()->StartTalking();
}

void AKHS_DronePlayer::CancelVoiceChat(const FInputActionValue& Value)
{
	GetController<AKJH_PlayerController>()->StopTalking();
}

#pragma endregion

#pragma region Image AI Object Detection

//태그를 전달받아 Actor를 검사할 함수
void AKHS_DronePlayer::CheckVisionForTags(const TArray<FString>& TagsToCheck)
{
	//새로운 태그가 전달되었을 때만 태그를 업데이트하고 감지 시작
	if (false == TagsToCheck.IsEmpty())
	{
		bIsTagSet = true;
		bIsCurrentlyDetecting = true;  // 감지 기능 활성화
	}

	//태그가 설정되지 않았거나 이미 감지 기능이 중지된 경우 함수 종료
	if (false == bIsTagSet || false == bIsCurrentlyDetecting)
	{
		return;
	}

	// 라인트레이스 범위 내에서 감지된 새로운 액터를 추적할 임시 변수
	TSet<AKHS_AIVisionObject*> CurrentlyDetectedAIVisionObjects;

	// 카메라 위치와 방향
	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();

	// 카메라의 시야각(FOV)을 기준으로 여러 방향으로 라인트레이스 발사
	float FOV = CameraComp->FieldOfView; // 카메라의 시야각
	int32 NumRays = 30; // 발사할 라인트레이스의 개수
	float MaxDistance = 5000.0f; // 라인트레이스의 최대 거리

	bool bAnyActorDetected = false;

	for (int32 i = 0; i < NumRays; i++)
	{
		// 시야각 범위 내에서 라인트레이스 방향 계산
		float Angle = FMath::Lerp(-FOV / 2, FOV / 2, (float)i / (float)(NumRays - 1));
		FRotator Rotator = FRotator(0, Angle, 0);
		FVector Direction = Rotator.RotateVector(ForwardVector);
		FVector End = Start + (Direction * MaxDistance);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);  // 자신을 무시

		// 라인트레이스 실행
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			AActor* HitActor = HitResult.GetActor();

			// 라인트레이스에 맞은 액터가 있는지 확인
			if (HitActor->IsA(AKHS_AIVisionObject::StaticClass()))
			{
				AKHS_AIVisionObject* HitAIVisionObject = Cast<AKHS_AIVisionObject>(HitActor);
				if (HitAIVisionObject)
				{
					// 전달된 모든 태그에 대해 체크
					for (const FString& Tag : TagsToCheck)
					{
						if (HitAIVisionObject->ActorHasTag(FName(*Tag)))
						{
							// 현재 감지된 액터를 임시 변수에 추가
							CurrentlyDetectedAIVisionObjects.Add(HitAIVisionObject);
							// 위젯 컴포넌트를 Visible로 설정
							if (HitAIVisionObject->WidgetComp)
							{
								HitAIVisionObject->WidgetComp->SetVisibility(true);
							}
							break; // 태그가 하나라도 일치하면 나머지 태그는 검사할 필요 없음
						}
						// 최소 하나의 액터가 감지됨
						bAnyActorDetected = true;
					}
				}
			}
		}
	}

	// 현재 태그와 일치하는 액터가 감지되지 않은 경우 감지 중지
	if (!bAnyActorDetected)
	{
		bIsCurrentlyDetecting = false;  // 감지 기능 비활성화
	}

	// 이전에 감지된 액터 중에서 이번 라인트레이스에서 감지되지 않은 액터를 찾아 UI를 숨김
	for (AKHS_AIVisionObject* DetectedObject : DetectedAIVisionObjects)
	{
		if (!CurrentlyDetectedAIVisionObjects.Contains(DetectedObject))
		{
			if (DetectedObject && DetectedObject->WidgetComp)
			{
				DetectedObject->WidgetComp->SetVisibility(false);
			}
		}
	}

	// 현재 감지된 액터들로 추적 세트 업데이트
	DetectedAIVisionObjects = CurrentlyDetectedAIVisionObjects;
}

// 텍스처를 JPEG 이미지로 저장하는 함수
void AKHS_DronePlayer::SaveCaptureToImage()
{
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("RenderTarget is null"));
		return;
	}

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	TArray<FColor> Bitmap;
	if (!RenderTargetResource->ReadPixels(Bitmap))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read pixels from render target"));
		return;
	}

	// Alpha 값을 255로 설정하여 불투명하게 만듭니다.
	for (FColor& Pixel : Bitmap)
	{
		Pixel.A = 255;
	}

	// 이미지 크기 설정
	int32 Width = RenderTarget->SizeX;
	int32 Height = RenderTarget->SizeY;

	// TArray64<FColor>로 Bitmap 배열 변환
	TArray64<FColor> Bitmap64;
	Bitmap64.Append(Bitmap);

	// 압축된 이미지 데이터
	TArray64<uint8> CompressedData;
	FImageUtils::PNGCompressImageArray(Width, Height, Bitmap64, CompressedData);

	// 파일 저장 경로 설정
	FString MainFileName = "CaptureImage";
	FString FileName = FString::Printf(TEXT("%s_%s.png"), *MainFileName, *FDateTime::Now().ToString());
	FString ImagePath = GetImagePath(FileName);

	// TArray64 데이터를 직접 파일로 저장
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*ImagePath);
	if (FileHandle)
	{
		FileHandle->Write(CompressedData.GetData(), CompressedData.Num());
		delete FileHandle;
		UE_LOG(LogTemp, Log, TEXT("Image saved to %s"), *ImagePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save image to %s"), *ImagePath);
	}

	// 서버로 전송 
	SendImageToServer(ImagePath, CompressedData);

	// Player State에 탐지요청횟수 증가
	auto* ps = GetPlayerState<AKJH_PlayerState>();
	if (ps)
	{
		ps->IncrementDroneDetectedCount();
	}

	// Capture 사운드를 1회 재생
	if (CaptureSFXFactory)
	{
		UGameplayStatics::PlaySound2D(this, CaptureSFXFactory);
	}

}

// 이미지 저장 경로를 설정하는 함수
FString AKHS_DronePlayer::GetImagePath(const FString& FileName) const
{
	// Captured 폴더 경로 설정
	FString CapturedFolderPath = FPaths::ProjectSavedDir() / TEXT("Captured");

	// Captured 폴더가 존재하지 않으면 생성
	if (!FPaths::DirectoryExists(CapturedFolderPath))
	{
		IFileManager::Get().MakeDirectory(*CapturedFolderPath, true);
	}

	// 파일 경로 반환
	return CapturedFolderPath / FileName;
}

// 이미지 전송 함수 (서버 전송 구현)
void AKHS_DronePlayer::SendImageToServer(const FString& ImagePath, const TArray64<uint8>& ImageData)
{
	UE_LOG(LogTemp, Log, TEXT("Sending image %s to server"), *ImagePath);

	// 1. HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();

	// 2. HTTP 요청 생성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	// 3. URL 설정 (여기서는 서버 주소를 설정합니다)
	//Request->SetURL(TEXT("localhost:3000/upload"));
	Request->SetURL(AIDetectionURL);

	// 4. HTTP 메소드 설정 (POST 방식)
	Request->SetVerb(TEXT("POST"));

	// 5. Multipart/FormData 헤더 설정
	FString Boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
	Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=") + Boundary);

	// 6. 파일의 컨텐츠를 multipart/form-data 형식으로 변환
	FString BeginBoundary = FString("--") + Boundary + TEXT("\r\n");
	FString EndBoundary = FString("--") + Boundary + TEXT("--\r\n");

	// 파일 정보 구성
	FString FileHeader = "Content-Disposition: form-data; name=\"file\"; filename=\"" + FPaths::GetCleanFilename(ImagePath) + "\"\r\n";
	FileHeader.Append("Content-Type: image/jpeg\r\n\r\n");

	// 전체 페이로드 구성 (문자열 부분과 바이너리 데이터 부분을 결합)
	FString PayloadString = BeginBoundary + FileHeader;
	TArray<uint8> Payload;
	Payload.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*PayloadString)), PayloadString.Len());
	Payload.Append(ImageData); // 이미지 데이터를 추가
	FString EndPayloadString = TEXT("\r\n") + EndBoundary;
	Payload.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*EndPayloadString)), EndPayloadString.Len());

	// 7. HTTP 요청에 내용 추가
	Request->SetContent(Payload);

	// 8. 요청 완료 시 콜백 함수 등록 (성공 여부 확인)
	Request->OnProcessRequestComplete().BindUObject(this, &AKHS_DronePlayer::OnResGetAIImage);

	// 9. HTTP 요청 보내기
	Request->ProcessRequest();
}

// AI에게 처리 이미지를 반환받을때 처리를 진행할 함수
void AKHS_DronePlayer::OnResGetAIImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//UE_LOG(LogTemp, Log, TEXT("Image uploaded successfully: %s"), *Response->GetContentAsString());
		
		//[수행작업1]
		//통신이 성공했을때 Response를 JsonLib를 통해 parsing한 결과를 배열에 담기.
		TArray<uint8> data = KHSJsonLib->JsonParseGetAIImage(Response->GetContentAsString());
		//데이터가 있을때
		if (data.Num() > 0)
		{ 
			//(선택) 파일 이름을 지정하여
			FString FileName = FString::Printf(TEXT("/Returned_%s.jpg"), *FDateTime::Now().ToString());
			//(선택) 파일 경로 지정 후
			FString imagePath = FPaths::ProjectPersistentDownloadDir() + FileName;
			//(선택) 내 쪽에 이미지가 잘 전달됐는지 확인 목적으로 저장
			FFileHelper::SaveArrayToFile(data, *imagePath);
			//(선택) 잘 저장됐는지 로그
			if (FFileHelper::SaveArrayToFile(data, *imagePath))
			{
				UE_LOG(LogTemp, Warning, TEXT("Returned Image saved to %s"), *imagePath);
			}
			//(선택) 저장 실패시 로그
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Image Saved Failed"));
				return;
			}
			// 받은 데이터를 UI Image로 전달하여 화면에 출력
			// 데이터를 Texture2D형태로 전환
			UTexture2D* realTexture = FImageUtils::ImportBufferAsTexture2D(data);
			if (realTexture)
			{
				// UI인스턴스를 만들어 UI Image에 접근
				UImage* AIImage = Cast<UImage>(DroneMainUI->GetWidgetFromName(TEXT("AIImage")));
				if (AIImage)
				{
					//UI 이미지에 텍스쳐 출력
					AIImage->SetBrushFromTexture(realTexture);
				}
				else
				{
					//텍스쳐 출력 실패시
					UE_LOG(LogTemp, Error, TEXT("Can not find AIImage Widget"));
				}
			}
			else
			{
				//Texture2D전환 실패시
				UE_LOG(LogTemp, Error, TEXT("UTexture2D Incoding Failed"));
			}
		}
		else
		{
			//Data Parsing실패시
			UE_LOG(LogTemp, Error, TEXT("No Parsing Image Data"));
		}

		//[수행작업2]
		// Json에서 감지된 태그들을 파싱하여 배열로 추출
		TArray<FString> DetectedTags = KHSJsonLib->JsonParseGetDetectedTags(Response->GetContentAsString());

		if (DetectedTags.Num() > 0)
		{
			// 추출된 태그들을 CheckVisionForTags 함수에 전달하여 처리
			CheckVisionForTags(DetectedTags);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No detected tags found in AI response"));
		}

		//[수행작업3] 
		//카테고리별 count 추출 및 PlayerState에 반영
		TArray<int32> DetectedCounts = KHSJsonLib->JsonParseGetDetectedCount(Response->GetContentAsString());

		if (DetectedCounts.Num() > 0)
		{
			// PlayerState에 각 카테고리의 카운트 반영
			AKJH_PlayerState* KJHPlayerState = GetPlayerState<AKJH_PlayerState>();
			if (KJHPlayerState)
			{
				for (int32 i = 0; i < DetectedCounts.Num(); ++i)
				{
					//결과배열의 모든 인덱스 카테고리에 접근해 해당 인덱스의 값을 전달한다.
					KJHPlayerState->IncrementDroneCategoryCount(i, DetectedCounts[i]);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No detected counts found in AI response"));
		}
	}
	else
	{
		//Image 전달 실패시
		UE_LOG(LogTemp, Error, TEXT("Image upload failed"));
	}
}

// SceneCaptureActor를 드론의 카메라와 같은 위치 및 각도로 동기화하는 함수
void AKHS_DronePlayer::SyncSceneCaptureWithCamera()
{
	if (SceneCaptureActor && CameraComp)
	{
		//UE_LOG(LogTemp, Log, TEXT("Syncing SceneCaptureActor with Camera."));
		// 카메라의 위치와 회전을 SceneCaptureActor에 복사
		SceneCaptureActor->SetActorLocationAndRotation(CameraComp->GetComponentLocation(), CameraComp->GetComponentRotation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneCaptureActor or CameraComp is null."));
	}
}

#pragma endregion

#pragma region STT AI ChatBot Function

// 서버로 오디오 파일 전송 함수
void AKHS_DronePlayer::SendAudioToServer(const FString& FilePath)
{
	// 프로젝트의 Saved/Recorded 폴더에 있는 "RecordedSound.wav" 파일 경로 설정
	FString SavedDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Recorded"));
	FString RecordedFilePath = FPaths::Combine(SavedDirectory, TEXT("RecordedSound.wav"));

	// 파일 데이터를 불러올 배열
	TArray<uint8> FileData;

	// 파일 데이터 로드 실패 시 로그 출력 및 함수 종료
	if (!FFileHelper::LoadFileToArray(FileData, *RecordedFilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File Data loading is failed: %s"), *RecordedFilePath);
		return;
	}

	// Boundary와 multipart/form-data 준비
	FString Boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
	FString BeginBoundary = FString("--") + Boundary + TEXT("\r\n");
	FString EndBoundary = FString("\r\n--") + Boundary + TEXT("--\r\n");

	// 파일 헤더 설정 (Content-Disposition, Content-Type 등)
	FString FileHeader = "Content-Disposition: form-data; name=\"audio\"; filename=\"" + FPaths::GetCleanFilename(RecordedFilePath) + "\"\r\n";
	FileHeader.Append("Content-Type: audio/wav\r\n\r\n");

	// multipart/form-data의 각 파트를 바이트 배열로 변환
	TArray<uint8> Payload;
	FString PayloadString = BeginBoundary + FileHeader;
	Payload.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*PayloadString)), PayloadString.Len());

	// 오디오 파일 데이터 추가
	Payload.Append(FileData);

	// 끝나는 경계를 추가
	FString EndPayloadString = EndBoundary;
	Payload.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*EndPayloadString)), EndPayloadString.Len());

	// HTTP 요청 생성 및 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(AIChatbotURL);  // 서버 URL을 설정
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=") + Boundary);
	Request->SetContent(Payload);

	// 요청 완료 시 콜백 함수 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &AKHS_DronePlayer::OnAudioUploadComplete);

	// 요청 전송
	if (Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Log, TEXT("Audio Wav File Uploading Request Success"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Audio Wav File Uploading Request Failed"));
	}
}

// 서버로 오디오 파일 업로드 완료 시 호출되는 콜백 함수
void AKHS_DronePlayer::OnAudioUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//UE_LOG(LogTemp, Log, TEXT("Audio File Uploading Success: %s"), *Response->GetContentAsString());

		// STT 콜백 함수 호출
		CallParsingAIText(Response->GetContentAsString());
		// STS 콜백 함수 호출
		CallParsingAISound(Response->GetContentAsString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Audio File Uploading Failed"));
	}
}

// STT 콜백 함수
void AKHS_DronePlayer::CallParsingAIText(const FString& json)
{
	// Json 파싱 후 텍스트 처리
	FString ParsedText = KHSJsonLib->JsonParseGetAIText(json); // Json에서 output_text 필드를 파싱
	//FString ParsedText = FString::Printf(TEXT("이것은 텍스트용 문장입니다. AI챗봇 최고랍니다. 여러분 모두 AI파트 정철이를 칭찬해주세요 진짜 이번 프로젝트 내내 협업능력 최고다 최고"));


	// UI에 텍스트 설정
	UTextBlock* AIChatText = Cast<UTextBlock>(DroneMainUI->GetWidgetFromName(TEXT("Text_AIChatResult")));
	if (AIChatText)
	{
		DisplayedText = FString::Printf(TEXT(""));  // 현재까지 표시된 텍스트를 빈 문자열로 초기화
		FullText = ParsedText;  // 전체 텍스트 저장
		CurrentCharIndex = 0;  // 현재 문자 인덱스 초기화

		// 타이머 설정 (0.1초 간격으로 UpdateDisplayedText 함수 호출)
		GetWorld()->GetTimerManager().SetTimer(TextDisplayTimerHandle, this, &AKHS_DronePlayer::UpdateDisplayedText, 0.1f, true);
		UE_LOG(LogTemp, Warning, TEXT("Starting text display animation..."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can not find Text_AIChatResult Widget in Drone Main UI"));
	}
	//// UI에 텍스트 설정
	//UTextBlock* AIChatText = Cast<UTextBlock>(DroneMainUI->GetWidgetFromName(TEXT("Text_AIChatResult")));
	//if (AIChatText)
	//{
	//	AIChatText->SetText(FText::FromString(ParsedText));
	//	UE_LOG(LogTemp, Warning, TEXT("Update TEXT_AIChatResult Widget"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Can not find Text_AIChatResult Widget in Drone Main UI"));
	//}
}

void AKHS_DronePlayer::UpdateDisplayedText()
{
	if (CurrentCharIndex < FullText.Len())
	{
		// 한 글자씩 추가하여 DisplayedText에 저장
		DisplayedText.AppendChar(FullText[CurrentCharIndex]);
		CurrentCharIndex++;

		// 텍스트 블럭에 업데이트
		UTextBlock* AIChatText = Cast<UTextBlock>(DroneMainUI->GetWidgetFromName(TEXT("Text_AIChatResult")));
		if (AIChatText)
		{
			AIChatText->SetText(FText::FromString(DisplayedText));

			// TypingSFXFactory 사운드를 1회 재생
			if (TypingSFXFactory)
			{
				UGameplayStatics::PlaySound2D(this, TypingSFXFactory);
			}
		}
	}
	else
	{
		// 모든 텍스트가 출력되었으면 타이머 중지
		GetWorld()->GetTimerManager().ClearTimer(TextDisplayTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Finished text display animation."));
	}
}

// STS 콜백 함수
void AKHS_DronePlayer::CallParsingAISound(const FString& json)
{
	// Json에서 output_audio 필드를 파싱
	TArray<uint8> AudioData = KHSJsonLib->JsonParseGetAIAudio(json);

	// 디코딩한 데이터가 있을 때만 처리
	if (AudioData.Num() > 0)
	{
		// 파일을 저장할 경로 설정
		FString SavedDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Recorded"));
		FString FilePath = FPaths::Combine(SavedDirectory, TEXT("AIAudioResponse.wav"));

		// 디렉토리가 존재하지 않으면 생성
		if (!FPaths::DirectoryExists(SavedDirectory))
		{
			FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SavedDirectory);
		}

		// 파일 저장
		if (!FFileHelper::SaveArrayToFile(AudioData, *FilePath))
		{
			UE_LOG(LogTemp, Error, TEXT("AI Response Audio File Save Failed : %s"), *FilePath);
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("AI Response Audio File Save Success: %s"), *FilePath);

		// 저장된 WAV 파일에서 데이터를 다시 읽어오기
		TArray<uint8> SoundData;
		if (!FFileHelper::LoadFileToArray(SoundData, *FilePath))
		{
			UE_LOG(LogTemp, Error, TEXT("File Data Loading Failed!: %s"), *FilePath);
			return;
		}
		if (SoundData.Num() < 44)
		{
			UE_LOG(LogTemp, Error, TEXT("WAV file is too small to contain a valid header."));
			return;
		}
		// WAV 파일의 크기 출력
		UE_LOG(LogTemp, Log, TEXT("WAV File Size: %d bytes"), SoundData.Num());

		// 첫 44바이트의 데이터를 로그로 출력 (WAV 헤더)
		/*for (int i = 0; i < 44; i++)
		{
			UE_LOG(LogTemp, Log, TEXT("Byte %d: %02x"), i, SoundData[i]);
		}*/
		
		// WAV 헤더에서 샘플 속도 및 채널 수 정보 추출
		int32 SampleRate=0;
		int16 NumChannels=0;
		int32 DataSize=0;

		// 바이트 배열에서 안전하게 값을 읽어오기 (리틀 엔디언을 고려)
		FMemory::Memcpy(&NumChannels, &SoundData[22], sizeof(int16));   // 채널 수
		FMemory::Memcpy(&SampleRate, &SoundData[24], sizeof(int32));    // 샘플 속도
		FMemory::Memcpy(&DataSize, &SoundData[40], sizeof(int32));      // 데이터 크기
		// 로그로 값 출력
		UE_LOG(LogTemp, Log, TEXT("SampleRate: %d, NumChannels: %d, DataSize: %d"), SampleRate, NumChannels, DataSize);
		// 값이 제대로 읽혀지지 않으면 오류 로그 출력
		if (NumChannels == 0 || SampleRate == 0 || DataSize == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse WAV header information. Please check the file format."));
			return;
		}

		// USoundWaveProcedural로 동적 사운드 웨이브 생성
		USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
		if (SoundWave)
		{
			// SoundWave 설정
			SoundWave->SetSampleRate(SampleRate);   // 샘플 속도 설정
			SoundWave->NumChannels = NumChannels;   // 채널 수 설정
			SoundWave->Duration = static_cast<float>(DataSize) / (SampleRate * NumChannels * sizeof(int16)); // 오디오 지속시간 설정
			SoundWave->bLooping = false; //(선택) 반복 여부 설정


			// WAV 파일의 44바이트 이후부터가 오디오 데이터이므로 그 이후의 데이터를 추출
			DataSize = SoundData.Num() - 44;  // 전체 데이터 크기에서 헤더 크기(44)를 뺀 값이 실제 PCM 데이터 크기입니다.
			TArray<uint8> PCMData(SoundData.GetData() + 44, DataSize);  // 44바이트 이후의 데이터를 추출

			if (PCMData.Num() > 0)
			{
				SoundWave->QueueAudio(PCMData.GetData(), PCMData.Num()); // 오디오 데이터를 SoundWave에 큐잉

				UE_LOG(LogTemp, Log, TEXT("SampleRate: %d, NumChannels: %d, DataSize: %d"), SampleRate, NumChannels, DataSize);
				
				// 오디오 재생을 위한 UAudioComponent 생성
				UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
				if (AudioComponent)
				{
					AudioComponent->SetSound(SoundWave);  // 생성된 SoundWave 설정
					AudioComponent->Play();  // 오디오 재생
					UE_LOG(LogTemp, Warning, TEXT("Playing AI Response Audio ..."));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("AudioComponent Create Failed"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PCM data doesn't exist"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SoundWave Create Failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No AI Response Audio Data Exist...."));
	}
}


void AKHS_DronePlayer::TestSound()
{
	// 파일을 저장할 경로 설정
	FString SavedDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Recorded"));
	FString FilePath = FPaths::Combine(SavedDirectory, TEXT("AIAudioResponse.wav"));

	// 디렉토리가 존재하지 않으면 생성
	if (!FPaths::DirectoryExists(SavedDirectory))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SavedDirectory);
	}

	// 저장된 WAV 파일에서 데이터를 다시 읽어오기
	TArray<uint8> SoundData;
	if (!FFileHelper::LoadFileToArray(SoundData, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File Data Loading Failed!: %s"), *FilePath);
		return;
	}
	if (SoundData.Num() < 44)
	{
		UE_LOG(LogTemp, Error, TEXT("WAV file is too small to contain a valid header."));
		return;
	}
	// WAV 파일의 크기 출력
	UE_LOG(LogTemp, Log, TEXT("WAV File Size: %d bytes"), SoundData.Num());

	// 첫 44바이트의 데이터를 로그로 출력 (WAV 헤더)
	for (int i = 0; i < 44; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Byte %d: %02x"), i, SoundData[i]);
	}
	// WAV 헤더에서 샘플 속도 및 채널 수 정보 추출
	int32 SampleRate = 0;
	int16 NumChannels = 0;
	int32 DataSize = 0;

	// 바이트 배열에서 안전하게 값을 읽어오기 (리틀 엔디언을 고려)
	FMemory::Memcpy(&NumChannels, &SoundData[22], sizeof(int16));   // 채널 수
	FMemory::Memcpy(&SampleRate, &SoundData[24], sizeof(int32));    // 샘플 속도
	FMemory::Memcpy(&DataSize, &SoundData[40], sizeof(int32));      // 데이터 크기
	// 로그로 값 출력
	UE_LOG(LogTemp, Log, TEXT("SampleRate: %d, NumChannels: %d, DataSize: %d"), SampleRate, NumChannels, DataSize);
	// 값이 제대로 읽혀지지 않으면 오류 로그 출력
	if (NumChannels == 0 || SampleRate == 0 || DataSize == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse WAV header information. Please check the file format."));
		return;
	}

	// USoundWaveProcedural로 동적 사운드 웨이브 생성
	USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
	if (SoundWave)
	{
		// SoundWave 설정
		SoundWave->SetSampleRate(SampleRate);   // 샘플 속도 설정
		SoundWave->NumChannels = NumChannels;   // 채널 수 설정
		SoundWave->Duration = static_cast<float>(DataSize) / (SampleRate * NumChannels * sizeof(int16)); // 오디오 지속시간 설정
		SoundWave->bLooping = false; //(선택) 반복 여부 설정


		// WAV 파일의 44바이트 이후부터가 오디오 데이터이므로 그 이후의 데이터를 추출
		DataSize = SoundData.Num() - 44;  // 전체 데이터 크기에서 헤더 크기(44)를 뺀 값이 실제 PCM 데이터 크기입니다.
		TArray<uint8> PCMData(SoundData.GetData() + 44, DataSize);  // 44바이트 이후의 데이터를 추출

		if (PCMData.Num() > 0)
		{
			SoundWave->QueueAudio(PCMData.GetData(), PCMData.Num()); // 오디오 데이터를 SoundWave에 큐잉

			UE_LOG(LogTemp, Log, TEXT("SampleRate: %d, NumChannels: %d, DataSize: %d"), SampleRate, NumChannels, DataSize);

			// 오디오 재생을 위한 UAudioComponent 생성
			UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
			if (AudioComponent)
			{
				AudioComponent->SetSound(SoundWave);  // 생성된 SoundWave 설정
				AudioComponent->Play();  // 오디오 재생
				UE_LOG(LogTemp, Warning, TEXT("Playing AI Response Audio ..."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AudioComponent Create Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PCM data doesn't exist"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SoundWave Create Failed"));
	}
}



#pragma endregion

