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
	//�ν��Ͻ�
	//==============================================

	//�̵��ӵ�
	UPROPERTY(EditDefaultsOnly)
	FVector DroneCurrentSpeed;

	//���ӵ� ����
	UPROPERTY(EditDefaultsOnly)
	FVector DroneAcceleration;

	//�ִ�ӵ�
	UPROPERTY(EditDefaultsOnly)
	float DroneMaxSpeed;

	//���ӵ� ������
	UPROPERTY(EditDefaultsOnly)
	float DroneAccelerateRate;

	//���ӵ�
	UPROPERTY(EditDefaultsOnly)
	float DroneDecelerateRate;

	//�̵�����
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
	//�Լ�
	//==============================================

	//��� �̵��Լ�
	void DroneMoveFwdPressed(const FInputActionValue& Value);
	void DroneMoveFwdReleased(const FInputActionValue& Value);
	void DroneMoveRightPressed(const FInputActionValue& Value);
	void DroneMoveRightReleased(const FInputActionValue& Value);
	void DroneMoveUpPressed(const FInputActionValue& Value);
	void DroneMoveUpReleased(const FInputActionValue& Value);
	void DroneMoveDownPressed(const FInputActionValue& Value);
	void DroneMoveDownReleased(const FInputActionValue& Value);


};
