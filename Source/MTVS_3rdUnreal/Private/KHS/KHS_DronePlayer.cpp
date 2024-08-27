// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DronePlayer.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

// Sets default values
AKHS_DronePlayer::AKHS_DronePlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;




	//Drone 스탯 초기화
	DroneMaxSpeed = 1000.0f;
	DroneAccelerateRate = 500.0f;
	DroneDecelerateRate = 800.0f;
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

}

// Called every frame
void AKHS_DronePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 속도에 가속도를 적용
	DroneCurrentSpeed += DroneAcceleration * DeltaTime;
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
}

// Called to bind functionality to input
void AKHS_DronePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_DroneFwd, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveFwdPressed);
		input->BindAction(IA_DroneFwd, ETriggerEvent::Completed, this, &AKHS_DronePlayer::DroneMoveFwdReleased);
		input->BindAction(IA_DroneRight, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveRightPressed);
		input->BindAction(IA_DroneRight, ETriggerEvent::Completed, this, &AKHS_DronePlayer::DroneMoveRightReleased);
		input->BindAction(IA_DroneUp, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveUpPressed);
		input->BindAction(IA_DroneUp, ETriggerEvent::Completed, this, &AKHS_DronePlayer::DroneMoveUpReleased);
		input->BindAction(IA_DroneDown, ETriggerEvent::Triggered, this, &AKHS_DronePlayer::DroneMoveDownPressed);
		input->BindAction(IA_DroneDown, ETriggerEvent::Completed, this, &AKHS_DronePlayer::DroneMoveDownReleased);

	}

}

void AKHS_DronePlayer::DroneMoveFwdPressed(const FInputActionValue& Value)
{
	float ForwardValue = Value.Get<float>();
	DroneAcceleration.X += ForwardValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveFwdReleased(const FInputActionValue& Value)
{
	DroneAcceleration.X = 0.0f; // 전진 가속도 멈춤
}

void AKHS_DronePlayer::DroneMoveRightPressed(const FInputActionValue& Value)
{
	float RightValue = Value.Get<float>();
	DroneAcceleration.Y += RightValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveRightReleased(const FInputActionValue& Value)
{
	DroneAcceleration.Y = 0.0f; // 좌우 가속도 멈춤
}

void AKHS_DronePlayer::DroneMoveUpPressed(const FInputActionValue& Value)
{
	float UpValue = Value.Get<float>();
	DroneAcceleration.Z += UpValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveUpReleased(const FInputActionValue& Value)
{
	DroneAcceleration.Z = 0.0f; // 상승 가속도 멈춤
}

void AKHS_DronePlayer::DroneMoveDownPressed(const FInputActionValue& Value)
{
	float DownValue = Value.Get<float>();
	DroneAcceleration.Z -= DownValue * DroneAccelerateRate;
}

void AKHS_DronePlayer::DroneMoveDownReleased(const FInputActionValue& Value)
{
	DroneAcceleration.Z = 0.0f; // 하강 가속도 멈춤
}
