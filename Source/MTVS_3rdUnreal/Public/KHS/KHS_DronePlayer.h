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

	//�浹ü, �޽�, ī�޶�
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCameraComponent* CameraComp;



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
    class UInputAction* IA_DroneLook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* IA_DroneFwd;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneRight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneUp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneDown;

	//MainUI �ν��Ͻ�
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DroneMainUIFactory;

	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* DroneMainUI;

	//ī�޶���ũ �ν��Ͻ�(������)
	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> DroneCameraShake;


	//==============================================
	//�Լ�
	//==============================================

	//��� �̵��Լ�
	void DroneLook(const FInputActionValue& Value);
	void DroneMoveFwd(const FInputActionValue& Value);
	void DroneMoveRight(const FInputActionValue& Value);
	void DroneMoveUp(const FInputActionValue& Value);
	void DroneMoveDown(const FInputActionValue& Value);

	//ī�޶���ũ ����Լ�
	void PlayDroneCameraShake();
};
