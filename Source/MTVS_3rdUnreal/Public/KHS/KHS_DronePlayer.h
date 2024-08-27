// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputLibrary.h"
#include "KHS_DronePlayer.generated.h"

UCLASS()
class MTVS_3RDUNREAL_API AKHS_DronePlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKHS_DronePlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//==============================================
	//인스턴스
	//==============================================

	//이동속도
	UPROPERTY(EditDefaultsOnly)
	FVector DroneCurrentSpeed;

	//가속도 변수
	UPROPERTY(EditDefaultsOnly)
	FVector DroneAcceleration;

	//최대속도
	UPROPERTY(EditDefaultsOnly)
	float DroneMaxSpeed;

	//가속도 증가율
	UPROPERTY(EditDefaultsOnly)
	float DroneAccelerateRate;

	//감속도
	UPROPERTY(EditDefaultsOnly)
	float DroneDecelerateRate;

	//이동방향
	FVector DroneDirection;

	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "input")
	class UInputMappingContext* IMC_Drone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* IA_DroneFwd;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneRight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneUp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneDown;

	//==============================================
	//함수
	//==============================================

	//드론 이동함수
	void DroneMoveFwdPressed(const FInputActionValue& Value);
	void DroneMoveFwdReleased(const FInputActionValue& Value);
	void DroneMoveRightPressed(const FInputActionValue& Value);
	void DroneMoveRightReleased(const FInputActionValue& Value);
	void DroneMoveUpPressed(const FInputActionValue& Value);
	void DroneMoveUpReleased(const FInputActionValue& Value);
	void DroneMoveDownPressed(const FInputActionValue& Value);
	void DroneMoveDownReleased(const FInputActionValue& Value);


};
