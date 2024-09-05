// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DronePlayer.h"
#include "KHS/KHS_DroneMainUI.h"
#include "KHS/KHS_AIVisionObject.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Net/VoiceConfig.h"
#include "TextureResource.h"
#include "Kismet/GameplayStatics.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/VoiceInterface.h"

// Sets default values
AKHS_DronePlayer::AKHS_DronePlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	////////// KJH 추가 Auto Possess 설정 추가 //////////
	AutoPossessPlayer = EAutoReceiveInput::Player0; // 첫 번째 플레이어에 대해 자동으로 Possess


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

	// VOIP Talker 컴포넌트를 생성하고, VOIPTalkerComponent 포인터에 할당합니다.
	VOIPTalkerComp = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalkerComp"));

	// 기본 렌더 타겟 생성 및 설정
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
	DroneMaxSpeed = 3000.0f;
	DroneAccelerateRate = 450.0f;
	DroneDecelerateRate = 400.0f;
	DroneCurrentSpeed = FVector::ZeroVector;
	DroneAcceleration = FVector::ZeroVector;
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
	
	//드론 Main UI초기화
	if(nullptr != DroneMainUI) 
	{ 
		DroneMainUI = CreateWidget(GetWorld(), DroneMainUIFactory);
		DroneMainUI->AddToViewport(0); 
	}

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
}

// Called every frame
void AKHS_DronePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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


	//Drone고도계 업데이트
	FVector s = GetActorLocation();
	FVector e = s - FVector(0, 0, 10000); //아래로 N만큼 라인트레이스 발사
	FHitResult HItResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HItResult,s,e,ECC_Visibility, params);
	if (bHit)
	{
		float Altitude = s.Z - HItResult.Location.Z;
		//UTextBlock* HeightText = Cast<UTextBlock>(DroneMainUI->GetWidgetFromName(TEXT("HeightText")));
		//HeightText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), Altitude)));
	}

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
	TArray<FString> TagsToCheck = { TEXT("Safe"), TEXT("Caution"), TEXT("Danger") };
	CheckVisionForTags(TagsToCheck);

	//// 매 프레임마다 감지된 액터를 체크
	//if (bIsCurrentlyDetecting)
	//{
	//	PeriodicallyCheckVision();
	//}

	//Post Process(Radial Blur, Depth of Field) 설정 함수
	SetDronePostProcess();

	// 매 프레임마다 SceneCaptureActor를 드론 카메라와 동기화
	SyncSceneCaptureWithCamera();

}

// Called to bind functionality to input
void AKHS_DronePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_DroneLook, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneLook);
		input->BindAction(IA_DroneFwd, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveFwd);
		input->BindAction(IA_DroneRight, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveRight);
		input->BindAction(IA_DroneUp, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveUp);
		input->BindAction(IA_DroneDown, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveDown);
		input->BindAction(IA_Function, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::SaveCaptureToImage);
		input->BindAction(IA_Voice, ETriggerEvent::Started, this, &AKHS_DronePlayer::SetUpNetworkVoice);
		input->BindAction(IA_Voice, ETriggerEvent::Completed, this, &AKHS_DronePlayer::StopVoice);
	}
}
//카메라쉐이크 재생함수
void AKHS_DronePlayer::PlayDroneCameraShake()
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && DroneCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DroneCameraShake);
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
					PlayerController->ConsoleCommand("OSS.VoiceLoopback 0");
				}
			}
			// 원격 Talker 등록
			RegisterRemoteTalker();

		}
	}
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

	// 서버로 전송 (네트워크 로직 추가 필요)
	SendImageToServer(ImagePath, CompressedData);
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
	//return FPaths::ProjectSavedDir() + FileName;
}

// 이미지 전송 함수 (서버 전송 구현)
void AKHS_DronePlayer::SendImageToServer(const FString& ImagePath, const TArray64<uint8>& ImageData)
{
	UE_LOG(LogTemp, Log, TEXT("Sending image %s to server"), *ImagePath);

	// HTTP 또는 WebSocket을 사용한 이미지 전송
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

//void AKHS_DronePlayer::CheckVisionForTag(FString Tag)
//{
//	//새로운 태그가 전달되었을 때만 태그를 업데이트하고 감지 시작
//	if (false == Tag.IsEmpty())
//	{
//		CurrentTag = Tag;
//		bIsTagSet = true;
//		bIsCurrentlyDetecting = true;  // 감지 기능 활성화
//	}
//
//	//태그가 설정되지 않았거나 이미 감지 기능이 중지된 경우 함수 종료
//	if (false == bIsTagSet || false == bIsCurrentlyDetecting)
//	{
//		return;
//	}
//
//	// 즉시 한번 감지 상태 확인
//	PeriodicallyCheckVision();
//}
//
//void AKHS_DronePlayer::PeriodicallyCheckVision()
//{
//	// 라인트레이스 범위 내에서 감지된 새로운 액터를 추적할 임시 변수
//	TSet<AKHS_AIVisionObject*> CurrentlyDetectedAIVisionObjects;
//
//	// 카메라 위치와 방향
//	FVector Start = CameraComp->GetComponentLocation();
//	FVector ForwardVector = CameraComp->GetForwardVector();
//
//	// 카메라의 시야각(FOV)을 기준으로 여러 방향으로 라인트레이스 발사
//	float FOV = CameraComp->FieldOfView; // 카메라의 시야각
//	int32 NumRays = 30; // 발사할 라인트레이스의 개수
//	float MaxDistance = 5000.0f; // 라인트레이스의 최대 거리
//
//	bool bAnyActorDetected = false;
//
//	for (int32 i = 0; i < NumRays; i++)
//	{
//		// 시야각 범위 내에서 라인트레이스 방향 계산
//		float Angle = FMath::Lerp(-FOV / 2, FOV / 2, (float)i / (float)(NumRays - 1));
//		FRotator Rotator = FRotator(0, Angle, 0);
//		FVector Direction = Rotator.RotateVector(ForwardVector);
//		FVector End = Start + (Direction * MaxDistance);
//		FHitResult HitResult;
//		FCollisionQueryParams CollisionParams;
//		CollisionParams.AddIgnoredActor(this);  // 자신을 무시
//
//		// 라인트레이스 실행
//		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
//		{
//			AActor* HitActor = HitResult.GetActor();
//
//			// 라인트레이스에 맞은 액터가 있는지 확인
//			if (HitActor->IsA(AKHS_AIVisionObject::StaticClass()))
//			{
//				AKHS_AIVisionObject* HitAIVisionObject = Cast<AKHS_AIVisionObject>(HitActor);
//				if (HitAIVisionObject)
//				{
//					// 액터의 태그가 전달받은 태그와 일치하는지 확인
//					if (HitAIVisionObject->ActorHasTag(FName(*CurrentTag)))
//					{
//						// 현재 감지된 액터를 임시 변수에 추가
//						CurrentlyDetectedAIVisionObjects.Add(HitAIVisionObject);
//
//						// 위젯 컴포넌트를 Visible로 설정
//						if (HitAIVisionObject->WidgetComp)
//						{
//							HitAIVisionObject->WidgetComp->SetVisibility(true);
//						}
//
//						// 최소 하나의 액터가 감지됨
//						bAnyActorDetected = true;
//					}
//				}
//			}
//		}
//	}
//
//	// 현재 태그와 일치하는 액터가 감지되지 않은 경우 감지 중지
//	if (!bAnyActorDetected)
//	{
//		bIsCurrentlyDetecting = false;  // 감지 기능 비활성화
//	}
//
//	// 이전에 감지된 액터 중에서 이번 라인트레이스에서 감지되지 않은 액터를 찾아 UI를 숨김
//	for (AKHS_AIVisionObject* DetectedObject : DetectedAIVisionObjects)
//	{
//		if (!CurrentlyDetectedAIVisionObjects.Contains(DetectedObject))
//		{
//			if (DetectedObject && DetectedObject->WidgetComp)
//			{
//				DetectedObject->WidgetComp->SetVisibility(false);
//			}
//		}
//	}
//
//	// 현재 감지된 액터들로 추적 세트 업데이트
//	DetectedAIVisionObjects = CurrentlyDetectedAIVisionObjects;
//}

#pragma endregion