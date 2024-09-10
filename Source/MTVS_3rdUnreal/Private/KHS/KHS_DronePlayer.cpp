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

#include "HttpModule.h"
#include "HttpFwd.h"


#include "Components/Image.h"

#include <KHS/KHS_JsonParseLib.h>

// Sets default values
AKHS_DronePlayer::AKHS_DronePlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//�浹ü, �޽�, ī�޶�
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

	// VOIP Talker ������Ʈ�� �����ϰ�, VOIPTalkerComponent �����Ϳ� �Ҵ��մϴ�.
	VOIPTalkerComp = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalkerComp"));

	// �̹��� ĸ�� �뵵 ���� Ÿ�� ���� �� ����
	RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	RenderTarget->InitAutoFormat(1280, 720); // ���ϴ� �ػ󵵷� �ʱ�ȭ
	
	//ī�޶���ũ ����
	DroneShakeInterval = 0.5f;  //0.5�ʸ��� ī�޶���ũ
	TimeSinceLastShake = 0.0f;

	//Hovering ����
	HoverAmplitude = 3.0f;
	HoverFrequency = 1.0f;
	RollAmplitude = 3.0f;
	RollFrequency = 1.0f;

	//Drone ���� �ʱ�ȭ
	//DroneMaxSpeed = 3000.0f;
	DroneMaxSpeed = 1500.0f;
	//DroneAccelerateRate = 450.0f;
	DroneAccelerateRate = 300.0f;
	//DroneDecelerateRate = 400.0f;
	DroneDecelerateRate = 250.0f;
	DroneCurrentSpeed = FVector::ZeroVector;
	DroneAcceleration = FVector::ZeroVector;

	//RPC���� �߰�
	SetReplicates(true);
	SetReplicateMovement(true);

	//SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	//SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//SphereComp->SetSimulatePhysics(true);  // ���� �ùķ��̼� Ȱ��ȭ
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
	
	////��� Main UI�ʱ�ȭ
	////���� �÷��̾��� ��쿡�� UI�����ϵ��� ����
	//if (IsLocallyControlled())
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

	//�޽� ��ġ, ȸ�� ����
	OriginalMeshLocation = MeshComp->GetRelativeLocation();
	OriginalMeshRotation = MeshComp->GetRelativeRotation();

	// VOIP �ʱ�ȭ �۾� ȣ��
	InitializeVOIP();

	// ���忡�� SceneCapture2D ���͸� ������
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
			// CameraComp�� SceneCaptureComponent�� Attach
			SceneCaptureComponent->AttachToComponent(CameraComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SceneCaptureComponent->TextureTarget = RenderTarget;
			SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
			SceneCaptureComponent->FOVAngle = CameraComp->FieldOfView; // ��� ī�޶�� ���� FOV
		}
		// �ʱ� SceneCaptureActor�� ������ ��� ī�޶�� ����ȭ
		SyncSceneCaptureWithCamera();
	}

}

// Called every frame
void AKHS_DronePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//���� �÷��̾ �ƴϸ� �۵� ����
	if (false == IsLocallyControlled())
	{
		return;
	}

	// Drone �̵� ó��
	FVector NewVelocity = DroneCurrentSpeed + (DroneAcceleration * DeltaTime);

	for (int32 i = 0; i < 3; i++)
	{
		// �ݴ� ������ ���ӵ��� ���� �� ������ ����
		if (FMath::Sign(DroneAcceleration[i]) != FMath::Sign(DroneCurrentSpeed[i]) && !FMath::IsNearlyZero(DroneCurrentSpeed[i]))
		{
			// ���� �ӵ��� ������ 0���� ���ҽ�Ŵ
			DroneCurrentSpeed[i] = FMath::FInterpTo(DroneCurrentSpeed[i], 0.0f, DeltaTime, DroneDecelerateRate);
		}
		else
		{
			// ���ӵ��� ���� �����̸� �Ϲ� ���ӵ� ����
			DroneCurrentSpeed[i] = NewVelocity[i];
		}

		// Ű �Է��� ���� �� ������ ���ߵ��� ó��
		if (DroneAcceleration.IsNearlyZero())
		{
			DroneCurrentSpeed[i] = FMath::FInterpTo(DroneCurrentSpeed[i], 0.0f, DeltaTime, DroneDecelerateRate);
		}
	}

	// �ִ� �ӵ� ����
	DroneCurrentSpeed = DroneCurrentSpeed.GetClampedToMaxSize(DroneMaxSpeed);

	// ��ġ ������Ʈ
	if (!DroneCurrentSpeed.IsNearlyZero())
	{
		FVector NewLocation = GetActorLocation() + (DroneCurrentSpeed * DeltaTime);
		SetActorLocation(NewLocation);
	}

	// �������� ���� �� ���ӵ� �ʱ�ȭ
	DroneAcceleration = FVector::ZeroVector;

	//���� ������Ʈ �Լ�
	DroneAltitudeUpdate();

	//�繰 ������ ǥ�� �Լ�
	DroneShowOutline();
	
	//ī�޶���ũ Ÿ�̸� ������Ʈ
	TimeSinceLastShake += DeltaTime;
	if (TimeSinceLastShake >= DroneShakeInterval)
	{
		PlayDroneCameraShake();
		TimeSinceLastShake = 0.0f; //Ÿ�̸� �ʱ�ȭ
	}

	// ���� Hovering ȿ�� ����
	float HoverOffset = HoverAmplitude * FMath::Sin(GetWorld()->TimeSeconds * HoverFrequency);
	FVector NewLocation = OriginalMeshLocation + FVector(0, 0, HoverOffset);
	MeshComp->SetRelativeLocation(NewLocation);

	// �¿� �ĵ� ȿ�� ����
	float RollOffset = RollAmplitude * FMath::Sin(GetWorld()->TimeSeconds * RollFrequency);
	float PitchOffset = RollAmplitude * FMath::Cos(GetWorld()->TimeSeconds * RollFrequency);
	FRotator NewRotation = OriginalMeshRotation + FRotator(PitchOffset, 0, RollOffset);
	MeshComp->SetRelativeRotation(NewRotation);

	// �þ߿� �ִ� ��� �±׸� ���� Actor�� �����ϰ� ���� ������Ʈ�� Ȱ��ȭ/��Ȱ��ȭ
	TArray<FString> TagsToCheck = { TEXT("Safe"), TEXT("Caution"), TEXT("Danger") };
	CheckVisionForTags(TagsToCheck);

	//// �� �����Ӹ��� ������ ���͸� üũ
	//if (bIsCurrentlyDetecting)
	//{
	//	PeriodicallyCheckVision();
	//}

	//Post Process(Radial Blur, Depth of Field) ���� �Լ�
	SetDronePostProcess();

	// �� �����Ӹ��� SceneCaptureActor�� ��� ī�޶�� ����ȭ
	SyncSceneCaptureWithCamera();

}

// Called to bind functionality to input
void AKHS_DronePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// DronePlayer�� ���� subsys �����(Client�� ��� �ʼ��ʿ�)
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
		input->BindAction(IA_Voice, ETriggerEvent::Started, this, &AKHS_DronePlayer::SetUpNetworkVoice);
		input->BindAction(IA_Voice, ETriggerEvent::Completed, this, &AKHS_DronePlayer::StopVoice);
	}
}
//����۽� Drone Player Possess�� �ٽ� �����
void AKHS_DronePlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); // �⺻ Ŭ������ Possess ������ �ٽ� �����Ͽ� Pawn�� Controller ���� ������ ����

	//��� Main UI�ʱ�ȭ
	//���� �÷��̾��� ��쿡�� UI�����ϵ��� ����
	if (IsLocallyControlled())
	{
		if (DroneMainUIFactory)
		{
			DroneMainUI = CreateWidget<UUserWidget>(GetWorld(), DroneMainUIFactory);
			if (DroneMainUI)
			{
				DroneMainUI->AddToViewport(0);
				UE_LOG(LogTemp, Warning, TEXT("Drone UI created for local player"));
			}
		}
	}


	// �Է� ������Ʈ �ʱ�ȭ
	// Possess ���Ŀ� �Է� ������Ʈ�� �ٽ� �����Ͽ� ����� �Է� ���ε��� �ùٸ��� �̷�������� ��.
	SetupPlayerInputComponent(NewController->InputComponent);

	// VOIP ���� �ʱ�ȭ �۾�
	// ����� ���� �����Ǿ��� �� �ʿ��� VOIP ���� ���� ���⼭ ���� (���̽�ä�� ����)
	InitializeVOIP();  // VOIP Talker�� �ʱ�ȭ �� ��� �۾��� ó��

	// �ʿ� �� �ٸ� �ʱ�ȭ �۾� �߰�
	// ��: ��� ī�޶� ����, UI �ʱ�ȭ, ��Ÿ ��Ʈ��ũ ���� ��

	// Enhanced Input �ý��ۿ� ����� �Է� ���� �߰�
	APlayerController* pc = Cast<APlayerController>(NewController);
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(IMC_Drone, 0); // �Է� ���� �߰�
		}
	}

}

#pragma region Drone Camera Effect
//ī�޶���ũ ����Լ�
void AKHS_DronePlayer::PlayDroneCameraShake()
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && DroneCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DroneCameraShake);
	}
}
//���� ������Ʈ �Լ�
void AKHS_DronePlayer::DroneAltitudeUpdate()
{
	// ����, �繰 ������ ǥ�� ����Ʈ���̽�
	FVector s = GetActorLocation();
	FVector e = s - FVector(0, 0, 10000); //�Ʒ��� N��ŭ ����Ʈ���̽� �߻�
	FHitResult HitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, s, e, ECC_Visibility, params);

	if (bHit)
	{
		//Drone ���� ������Ʈ
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
//Post Process(Radial Blur, Depth of Field) ���� �Լ�
void AKHS_DronePlayer::SetDronePostProcess()
{
	// ����Ʈ���̽� �� ���� �Ÿ� ���
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

	// Radial Blur Material�� ī�޶� ����� �������� Ȯ��
	if (RadialBlurMaterial && MPC_DroneBlur)
	{
		// ����� �ӵ��� ���� Radial Blur ����
		float CurrentSpeed = DroneCurrentSpeed.Size();
		float SpeedBlurAmount = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, DroneMaxSpeed), FVector2D(0.12f, 0.25f), CurrentSpeed);

		// Material Parameter Collection�� �ӵ� ���� ����
		UMaterialParameterCollectionInstance* MaterialParamInstance = GetWorld()->GetParameterCollectionInstance(MPC_DroneBlur);
		if (MaterialParamInstance)
		{
			MaterialParamInstance->SetScalarParameterValue(FName("BlurAmount"), SpeedBlurAmount);
		}

		// Radial Blur�� ����Ʈ ���μ��� ������ �ݿ� 
		PostProcessSettings.bOverride_BloomIntensity = true;
		PostProcessSettings.BloomIntensity = 1;//SpeedBlurAmount;

		// ����Ʈ ���μ��� ���� ������Ʈ(Depth of Field)
		PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
		PostProcessSettings.DepthOfFieldFocalDistance = FocusDistance;

		// Aperture (F-Stop) ���� ����
		PostProcessSettings.bOverride_DepthOfFieldFstop = true;
		PostProcessSettings.DepthOfFieldFstop = 1.0f;  // F-Stop �� ����

		// Maximum Aperture (Min F-Stop) ���� ����
		PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
		PostProcessSettings.DepthOfFieldMinFstop = 11.0f;  // Min Aperture �� ����

		// ����� ī�޶� ����Ʈ ���μ��� ���� ����
		CameraComp->PostProcessSettings = PostProcessSettings;

		FWeightedBlendable Blendable;
		Blendable.Object = RadialBlurMaterial;
		Blendable.Weight = 1.0f; // �ʿ信 ���� ���� ����
		CameraComp->PostProcessSettings.WeightedBlendables.Array.Add(Blendable);

		//�ӵ��� ���
		GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, FString::Printf(TEXT("Speed : %f"), CurrentSpeed));
	}
}
//Drone Outline PostProcess ȿ�� �Լ�
void AKHS_DronePlayer::DroneShowOutline()
{
	// ����Ʈ���̽� ���� ������ ������ ���ο� ���͸� ������ �ӽ� ����
	TSet<AKHS_AIVisionObject*> DetectedOutlineObjects;

	// ī�޶� ��ġ�� ����
	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();

	// ī�޶��� �þ߰�(FOV)�� �������� ���� �������� ����Ʈ���̽� �߻�
	float FOV = CameraComp->FieldOfView; // ī�޶��� �þ߰�
	int32 NumRays = 30; // �߻��� ����Ʈ���̽��� ����
	float MaxDistance = 10000.0f; // ����Ʈ���̽��� �ִ� �Ÿ�


	for (int32 i = 0; i < NumRays; i++)
	{
		// �þ߰� ���� ������ ����Ʈ���̽� ���� ���
		float Angle = FMath::Lerp(-FOV / 2, FOV / 2, (float)i / (float)(NumRays - 1));
		FRotator Rotator = FRotator(0, Angle, 0);
		FVector Direction = Rotator.RotateVector(ForwardVector);
		FVector End = Start + (Direction * MaxDistance);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);  // �ڽ��� ����

		// ����Ʈ���̽� ����
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor && HitActor->IsA(AKHS_AIVisionObject::StaticClass()))
			{
				AKHS_AIVisionObject* HitAIVisionObject = Cast<AKHS_AIVisionObject>(HitActor);

				// ���� ������ ���͸� �ӽ� ������ �߰�
				DetectedOutlineObjects.Add(HitAIVisionObject);

				// ���� ���Ͱ� KHS_AIVisionObject�� ��� ������ ����
				DroneEnableOutline(HitActor);
			}
			else
			{
				// ������ ȿ�� ����
				DroneDisableOutline(HitActor);
			}

		}
	}

	// ������ ������ ���� �߿��� �̹� ����Ʈ���̽����� �������� ���� ���͸� ã�� ������ ����
	for (AKHS_AIVisionObject* DetectedObject : DetectedAIVisionObjects)
	{
		if (!DetectedOutlineObjects.Contains(DetectedObject))
		{
			// ������ ȿ�� ����
			DroneDisableOutline(DetectedObject);
		}
	}

	// ���� ������ ���͵�� ���� ��Ʈ ������Ʈ
	//DetectedAIVisionObjects = DetectedOutlineObjects;
}
// ����Ʈ���̽� ��� ������ ǥ�� �Լ�
void AKHS_DronePlayer::DroneEnableOutline(AActor* HitActor)
{
	if (HitActor && HitActor->IsA(AKHS_AIVisionObject::StaticClass())) // Actor�� KHS_AIVisionObject Ÿ������ Ȯ��
	{
		UStaticMeshComponent* HitMeshComp = Cast<UStaticMeshComponent>(HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (HitMeshComp)
		{
			HitMeshComp->SetRenderCustomDepth(true);
			HitMeshComp->CustomDepthStencilValue = 1; // ���ٽ� ���� 1�� ����
		}
	}
}
// ����Ʈ���̽� ��� �þ� ������� ������ ���� �Լ�
void AKHS_DronePlayer::DroneDisableOutline(AActor* HitActor)
{
	if (HitActor && HitActor->IsA(AKHS_AIVisionObject::StaticClass()))  // Actor�� KHS_AIVisionObject Ÿ������ Ȯ��
	{
		UStaticMeshComponent* HitMeshComp = Cast<UStaticMeshComponent>(HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (HitMeshComp)
		{
			// CustomDepth�� ��Ȱ��ȭ�Ͽ� ������ ȿ�� ����
			HitMeshComp->SetRenderCustomDepth(false);
		}
	}
}
#pragma endregion

#pragma region Drone Move Settings

void AKHS_DronePlayer::DroneLook(const FInputActionValue& Value)
{
	// �Էµ� 2D ���� ���� ������ ī�޶��� Yaw(X) �� Pitch(Y) �� ����
	FVector2D v = Value.Get<FVector2D>();
	// Yaw(�¿� ȸ��) �Է� ó��
	AddControllerYawInput(v.X);
	// Pitch(���� ȸ��) �Է� ó��
	AddControllerPitchInput(-v.Y);
}

void AKHS_DronePlayer::DroneMoveFwd(const FInputActionValue& Value)
{
	float ForwardValue = Value.Get<float>();

	// ī�޶��� ������ ������� �̵� ���͸� ���
	FRotator ControlRotation = GetControlRotation();
	FRotator YawRotation(0, ControlRotation.Yaw, 0); // Pitch �����Ͽ� Yaw�� ���
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);

	// ��� ���ӵ��� ī�޶��� �� ������ �������� ����
	DroneAcceleration += ForwardDirection * ForwardValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveRight(const FInputActionValue& Value)
{
	float RightValue = Value.Get<float>();

	// ī�޶��� ������ ������� �̵� ���͸� ���
	FRotator ControlRotation = GetControlRotation();
	FRotator YawRotation(0, ControlRotation.Yaw, 0); // Pitch �����Ͽ� Yaw�� ���
	FVector RightDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	// ��� ���ӵ��� ī�޶��� ������ ������ �������� ����
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

//����ũ �Ӱ谪 ����
void AKHS_DronePlayer::SetMicThreshold(float Threshold)
{
	if (VOIPTalkerComp)
	{
		UVOIPStatics::SetMicThreshold(Threshold);
	}
}
//�÷��̾� State ���
void AKHS_DronePlayer::RegisterWithPlayerState()
{
	if (VOIPTalkerComp && GetPlayerState())
	{
		VOIPTalkerComp->RegisterWithPlayerState(GetPlayerState());
	}
}
//���� �÷��̾ ���������� üũ
bool AKHS_DronePlayer::IsLocallyControlled() const
{
	return IsPlayerControlled();
}

//StartNetworkVoice ��Ʈ��ũ�� ���带 ����
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
					// �÷��̾ Voice Channel �Ҵ�
					VoiceInterface->StartNetworkedVoice(PlayerController->GetLocalPlayer()->GetControllerId());
				}
			}
		}
	}
}

//StopNetworkVoice ��Ʈ��ũ�� ���� ������ ����
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

//VOIP ����� ���
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
				// �÷��̾��� ���� ��Ʈ��ũ ID�� �����ɴϴ�.
				TSharedPtr<const FUniqueNetId> UniqueNetId = PlayerController->PlayerState->UniqueId.GetUniqueNetId();

				if (UniqueNetId.IsValid())
				{
					// ���� ��Ʈ��ũ ID�� ����Ͽ� ���� Talker�� ����մϴ�.
					VoiceInterface->RegisterRemoteTalker(*UniqueNetId);
				}
			}
		}
	}
}

//VOIP���� �ʱ�ȭ �۾�
void AKHS_DronePlayer::InitializeVOIP()
{
	if (VOIPTalkerComp)
	{
		// VOIPTalkerComponent�� ��ȿ���� Ȯ��
		if (IsValid(VOIPTalkerComp))
		{
			// �÷��̾� ���¿� VOIPTalker�� ���
			RegisterWithPlayerState();

			// ����ũ �Ӱ谪�� ����
			SetMicThreshold(-1.0f);

			// ���� �÷��̾ ���� ���� ���� VOIP ���� ������ ����
			if (IsLocallyControlled())
			{
				// �ܼ� ����� �����Ͽ� VOIP�� Ȱ��ȭ
				APlayerController* PlayerController = Cast<APlayerController>(GetController());
				if (PlayerController)
				{
					PlayerController->ConsoleCommand("OSS.VoiceLoopback 1");
				}
			}
			// ���� Talker ���
			//RegisterRemoteTalker();

		}
	}
}

#pragma endregion

#pragma region Image AI Object Detection

//�±׸� ���޹޾� Actor�� �˻��� �Լ�
void AKHS_DronePlayer::CheckVisionForTags(const TArray<FString>& TagsToCheck)
{
	//���ο� �±װ� ���޵Ǿ��� ���� �±׸� ������Ʈ�ϰ� ���� ����
	if (false == TagsToCheck.IsEmpty())
	{
		bIsTagSet = true;
		bIsCurrentlyDetecting = true;  // ���� ��� Ȱ��ȭ
	}

	//�±װ� �������� �ʾҰų� �̹� ���� ����� ������ ��� �Լ� ����
	if (false == bIsTagSet || false == bIsCurrentlyDetecting)
	{
		return;
	}

	// ����Ʈ���̽� ���� ������ ������ ���ο� ���͸� ������ �ӽ� ����
	TSet<AKHS_AIVisionObject*> CurrentlyDetectedAIVisionObjects;

	// ī�޶� ��ġ�� ����
	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();

	// ī�޶��� �þ߰�(FOV)�� �������� ���� �������� ����Ʈ���̽� �߻�
	float FOV = CameraComp->FieldOfView; // ī�޶��� �þ߰�
	int32 NumRays = 30; // �߻��� ����Ʈ���̽��� ����
	float MaxDistance = 5000.0f; // ����Ʈ���̽��� �ִ� �Ÿ�

	bool bAnyActorDetected = false;

	for (int32 i = 0; i < NumRays; i++)
	{
		// �þ߰� ���� ������ ����Ʈ���̽� ���� ���
		float Angle = FMath::Lerp(-FOV / 2, FOV / 2, (float)i / (float)(NumRays - 1));
		FRotator Rotator = FRotator(0, Angle, 0);
		FVector Direction = Rotator.RotateVector(ForwardVector);
		FVector End = Start + (Direction * MaxDistance);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);  // �ڽ��� ����

		// ����Ʈ���̽� ����
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			AActor* HitActor = HitResult.GetActor();

			// ����Ʈ���̽��� ���� ���Ͱ� �ִ��� Ȯ��
			if (HitActor->IsA(AKHS_AIVisionObject::StaticClass()))
			{
				AKHS_AIVisionObject* HitAIVisionObject = Cast<AKHS_AIVisionObject>(HitActor);
				if (HitAIVisionObject)
				{
					// ���޵� ��� �±׿� ���� üũ
					for (const FString& Tag : TagsToCheck)
					{
						if (HitAIVisionObject->ActorHasTag(FName(*Tag)))
						{
							// ���� ������ ���͸� �ӽ� ������ �߰�
							CurrentlyDetectedAIVisionObjects.Add(HitAIVisionObject);
							// ���� ������Ʈ�� Visible�� ����
							if (HitAIVisionObject->WidgetComp)
							{
								HitAIVisionObject->WidgetComp->SetVisibility(true);
							}
							break; // �±װ� �ϳ��� ��ġ�ϸ� ������ �±״� �˻��� �ʿ� ����
						}
						// �ּ� �ϳ��� ���Ͱ� ������
						bAnyActorDetected = true;
					}
				}
			}
		}
	}

	// ���� �±׿� ��ġ�ϴ� ���Ͱ� �������� ���� ��� ���� ����
	if (!bAnyActorDetected)
	{
		bIsCurrentlyDetecting = false;  // ���� ��� ��Ȱ��ȭ
	}

	// ������ ������ ���� �߿��� �̹� ����Ʈ���̽����� �������� ���� ���͸� ã�� UI�� ����
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

	// ���� ������ ���͵�� ���� ��Ʈ ������Ʈ
	DetectedAIVisionObjects = CurrentlyDetectedAIVisionObjects;
}

// �ؽ�ó�� JPEG �̹����� �����ϴ� �Լ�
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

	// Alpha ���� 255�� �����Ͽ� �������ϰ� ����ϴ�.
	for (FColor& Pixel : Bitmap)
	{
		Pixel.A = 255;
	}

	// �̹��� ũ�� ����
	int32 Width = RenderTarget->SizeX;
	int32 Height = RenderTarget->SizeY;

	// TArray64<FColor>�� Bitmap �迭 ��ȯ
	TArray64<FColor> Bitmap64;
	Bitmap64.Append(Bitmap);

	// ����� �̹��� ������
	TArray64<uint8> CompressedData;
	FImageUtils::PNGCompressImageArray(Width, Height, Bitmap64, CompressedData);

	// ���� ���� ��� ����
	FString MainFileName = "CaptureImage";
	FString FileName = FString::Printf(TEXT("%s_%s.png"), *MainFileName, *FDateTime::Now().ToString());
	FString ImagePath = GetImagePath(FileName);

	// TArray64 �����͸� ���� ���Ϸ� ����
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

	// ������ ���� (��Ʈ��ũ ���� �߰� �ʿ�)
	SendImageToServer(ImagePath, CompressedData);
}

// �̹��� ���� ��θ� �����ϴ� �Լ�
FString AKHS_DronePlayer::GetImagePath(const FString& FileName) const
{
	// Captured ���� ��� ����
	FString CapturedFolderPath = FPaths::ProjectSavedDir() / TEXT("Captured");

	// Captured ������ �������� ������ ����
	if (!FPaths::DirectoryExists(CapturedFolderPath))
	{
		IFileManager::Get().MakeDirectory(*CapturedFolderPath, true);
	}

	// ���� ��� ��ȯ
	return CapturedFolderPath / FileName;
}

// �̹��� ���� �Լ� (���� ���� ����)
void AKHS_DronePlayer::SendImageToServer(const FString& ImagePath, const TArray64<uint8>& ImageData)
{
	UE_LOG(LogTemp, Log, TEXT("Sending image %s to server"), *ImagePath);

	// 1. HTTP ��� ��������
	FHttpModule* Http = &FHttpModule::Get();

	// 2. HTTP ��û ����
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	// 3. URL ���� (���⼭�� ���� �ּҸ� �����մϴ�)
	//Request->SetURL(TEXT("localhost:3000/upload"));
	Request->SetURL(AIDetectionURL);

	// 4. HTTP �޼ҵ� ���� (POST ���)
	Request->SetVerb(TEXT("POST"));

	// 5. Multipart/FormData ��� ����
	FString Boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
	Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=") + Boundary);

	// 6. ������ �������� multipart/form-data �������� ��ȯ
	FString BeginBoundary = FString("--") + Boundary + TEXT("\r\n");
	FString EndBoundary = FString("--") + Boundary + TEXT("--\r\n");

	// ���� ���� ����
	FString FileHeader = "Content-Disposition: form-data; name=\"file\"; filename=\"" + FPaths::GetCleanFilename(ImagePath) + "\"\r\n";
	FileHeader.Append("Content-Type: image/jpeg\r\n\r\n");

	// ��ü ���̷ε� ���� (���ڿ� �κа� ���̳ʸ� ������ �κ��� ����)
	FString PayloadString = BeginBoundary + FileHeader;
	TArray<uint8> Payload;
	Payload.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*PayloadString)), PayloadString.Len());
	Payload.Append(ImageData); // �̹��� �����͸� �߰�
	FString EndPayloadString = TEXT("\r\n") + EndBoundary;
	Payload.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*EndPayloadString)), EndPayloadString.Len());

	// 7. HTTP ��û�� ���� �߰�
	Request->SetContent(Payload);

	// 8. ��û �Ϸ� �� �ݹ� �Լ� ��� (���� ���� Ȯ��)
	Request->OnProcessRequestComplete().BindUObject(this, &AKHS_DronePlayer::OnResGetAIImage);

	// 9. HTTP ��û ������
	Request->ProcessRequest();
}

// AI���� ó�� �̹����� ��ȯ������ ó���� ������ �Լ�
void AKHS_DronePlayer::OnResGetAIImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Image uploaded successfully: %s"), *Response->GetContentAsString());
		
		//����� ���������� Response�� JsonLib�� ���� parsing�� ����� �迭�� ���.
		TArray<uint8> data = KHSJsonLib->JsonParseGetAIImage(Response->GetContentAsString());
		//�����Ͱ� ������
		if (data.Num() > 0)
		{
			//(����) ���� �̸��� �����Ͽ�
			FString FileName = FString::Printf(TEXT("/Returned_%s.jpg"), *FDateTime::Now().ToString());
			//(����) ���� ��� ���� ��
			FString imagePath = FPaths::ProjectPersistentDownloadDir() + FileName;
			//(����) �� �ʿ� �̹����� �� ���޵ƴ��� Ȯ�� �������� ����
			FFileHelper::SaveArrayToFile(data, *imagePath);
			//(����) �� ����ƴ��� �α�
			if (FFileHelper::SaveArrayToFile(data, *imagePath))
			{
				UE_LOG(LogTemp, Warning, TEXT("Returned Image saved to %s"), *imagePath);
			}
			//(����) ���� ���н� �α�
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Image Saved Failed"));
				return;
			}
			// ���� �����͸� UI Image�� �����Ͽ� ȭ�鿡 ���
			// �����͸� Texture2D���·� ��ȯ
			UTexture2D* realTexture = FImageUtils::ImportBufferAsTexture2D(data);
			if (realTexture)
			{
				// UI�ν��Ͻ��� ����� UI Image�� ����
				UImage* AIImage = Cast<UImage>(DroneMainUI->GetWidgetFromName(TEXT("AIImage")));
				if (AIImage)
				{
					//UI �̹����� �ؽ��� ���
					AIImage->SetBrushFromTexture(realTexture);
				}
				else
				{
					//�ؽ��� ��� ���н�
					UE_LOG(LogTemp, Error, TEXT("Can not find AIImage Widget"));
				}
			}
			else
			{
				//Texture2D��ȯ ���н�
				UE_LOG(LogTemp, Error, TEXT("UTexture2D Incoding Failed"));
			}
		}
		else
		{
			//Data Parsing���н�
			UE_LOG(LogTemp, Error, TEXT("No Parsing Image Data"));
		}
	}
	else
	{
		//Image ���� ���н�
		UE_LOG(LogTemp, Error, TEXT("Image upload failed"));
	}
}

// SceneCaptureActor�� ����� ī�޶�� ���� ��ġ �� ������ ����ȭ�ϴ� �Լ�
void AKHS_DronePlayer::SyncSceneCaptureWithCamera()
{
	if (SceneCaptureActor && CameraComp)
	{
		//UE_LOG(LogTemp, Log, TEXT("Syncing SceneCaptureActor with Camera."));
		// ī�޶��� ��ġ�� ȸ���� SceneCaptureActor�� ����
		SceneCaptureActor->SetActorLocationAndRotation(CameraComp->GetComponentLocation(), CameraComp->GetComponentRotation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneCaptureActor or CameraComp is null."));
	}
}

#pragma endregion



