// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DronePlayer.h"
#include "KHS/KHS_DroneMainUI.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Net/VoiceConfig.h"
#include "KHS/KHS_AIVisionObject.h"

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

	//Post Process Radial Blur 강도 결정 변수
	MPC_DroneBlur = LoadObject<UMaterialParameterCollection>(nullptr, TEXT("/Script/Engine.MaterialParameterCollection'/Game/Blueprints/UI/KHS/MPC_DroneBlur.MPC_DroneBlur'"));

	// VOIP 초기화 작업 호출
	InitializeVOIP();
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

	//Post Process Radial Blur효과 강도 결정(속도에 따라)
	// 드론의 현재 속도 계산
	//float CurrentSpeed = GetVelocity().Size();
	float CurrentSpeed = DroneCurrentSpeed.Size();
	GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, FString::Printf(TEXT("Speed : %f"), CurrentSpeed));

	// BlurAmount를 속도에 따라 설정
	float SpeedBlurAmount = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, DroneMaxSpeed), FVector2D(0.12f, 0.25f), CurrentSpeed);

	// Material Parameter Collection에 속도값전달
	UMaterialParameterCollectionInstance* MaterialParamInstance = GetWorld()->GetParameterCollectionInstance(MPC_DroneBlur);
	if (MaterialParamInstance)
	{
		MaterialParamInstance->SetScalarParameterValue(FName("BlurAmount"), SpeedBlurAmount);
	}

	

	// 시야에 있는 특정 태그를 가진 Actor를 감지하고 위젯 컴포넌트를 활성화/비활성화
	FString DesiredTag = TEXT("Danger");  // 검사할 태그 설정
	CheckVisionForTag(DesiredTag);

	//// 매 프레임마다 감지된 액터를 체크
	//if (bIsCurrentlyDetecting)
	//{
	//	PeriodicallyCheckVision();
	//}


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
	}
}

void AKHS_DronePlayer::PlayDroneCameraShake()
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && DroneCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DroneCameraShake);
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
		}
	}
}

#pragma endregion


void AKHS_DronePlayer::CheckVisionForTag(FString Tag)
{
	//새로운 태그가 전달되었을 때만 태그를 업데이트하고 감지 시작
	if (false == Tag.IsEmpty())
	{
		CurrentTag = Tag;
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
					// 액터의 태그가 전달받은 태그와 일치하는지 확인
					if (HitAIVisionObject->ActorHasTag(FName(*CurrentTag)))
					{
						// 현재 감지된 액터를 임시 변수에 추가
						CurrentlyDetectedAIVisionObjects.Add(HitAIVisionObject);

						// 위젯 컴포넌트를 Visible로 설정
						if (HitAIVisionObject->WidgetComp)
						{
							HitAIVisionObject->WidgetComp->SetVisibility(true);
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