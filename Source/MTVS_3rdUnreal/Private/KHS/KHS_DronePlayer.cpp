// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DronePlayer.h"
#include "KHS/KHS_DroneMainUI.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

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

	//ī�޶���ũ ����
	DroneShakeInterval = 0.5f;  //0.5�ʸ��� ī�޶���ũ
	TimeSinceLastShake = 0.0f;

	//Hovering ����
	HoverAmplitude = 3.0f;
	HoverFrequency = 1.0f;
	RollAmplitude = 3.0f;
	RollFrequency = 1.0f;

	//Drone ���� �ʱ�ȭ
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
	
	//��� Main UI�ʱ�ȭ
	if(nullptr != DroneMainUI) 
	{ 
		DroneMainUI = CreateWidget(GetWorld(), DroneMainUIFactory);
		DroneMainUI->AddToViewport(0); 
	}

	//�޽� ��ġ, ȸ�� ����
	OriginalMeshLocation = MeshComp->GetRelativeLocation();
	OriginalMeshRotation = MeshComp->GetRelativeRotation();

	//Post Process Radial Blur ���� ���� ����
	MPC_DroneBlur = LoadObject<UMaterialParameterCollection>(nullptr, TEXT("/Script/Engine.MaterialParameterCollection'/Game/Blueprints/UI/KHS/MPC_DroneBlur.MPC_DroneBlur'"));


}

// Called every frame
void AKHS_DronePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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


	//Drone���� ������Ʈ
	FVector s = GetActorLocation();
	FVector e = s - FVector(0, 0, 10000); //�Ʒ��� N��ŭ ����Ʈ���̽� �߻�
	FHitResult HItResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HItResult,s,e,ECC_Visibility, params);
	if (bHit)
	{
		float Altitude = s.Z - HItResult.Location.Z;
		UTextBlock* HeightText = Cast<UTextBlock>(DroneMainUI->GetWidgetFromName(TEXT("HeightText")));
		HeightText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), Altitude)));
	}

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

	//Post Process Radial Blurȿ�� ���� ����(�ӵ��� ����)
	// ����� ���� �ӵ� ���
	//float CurrentSpeed = GetVelocity().Size();
	float CurrentSpeed = DroneCurrentSpeed.Size();
	GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, FString::Printf(TEXT("Speed : %f"), CurrentSpeed));

	// BlurAmount�� �ӵ��� ���� ����
	float SpeedBlurAmount = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, DroneMaxSpeed), FVector2D(0.12f, 0.25f), CurrentSpeed);

	// Material Parameter Collection�� �ӵ�������
	UMaterialParameterCollectionInstance* MaterialParamInstance = GetWorld()->GetParameterCollectionInstance(MPC_DroneBlur);
	if (MaterialParamInstance)
	{
		MaterialParamInstance->SetScalarParameterValue(FName("BlurAmount"), SpeedBlurAmount);
	}

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

void AKHS_DronePlayer::PlayDroneCameraShake()
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && DroneCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DroneCameraShake);
	}
}