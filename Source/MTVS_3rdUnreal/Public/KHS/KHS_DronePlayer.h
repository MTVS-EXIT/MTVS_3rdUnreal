// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputLibrary.h"
#include "Engine/Scene.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCameraComponent* CameraComp;

	// VOIP Talker ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice Chat", meta = (AllowPrivateAccess = "true"))
	class UVOIPTalker* VOIPTalkerComp;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Function;

	//MainUI �ν��Ͻ�
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DroneMainUIFactory;

	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* DroneMainUI;

	//AI HUD UI ���� ���� Set

	//ī�޶���ũ �ν��Ͻ�(������)
	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> DroneCameraShake;
	//����ũ �ֱ�
	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	float DroneShakeInterval;
	//������ ����ũ �� ����ð�
	float TimeSinceLastShake;

	//Hovering �޽� ��鸲 ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float HoverAmplitude; //������鸲 ����(�޽ð� �����̴� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float HoverFrequency; //������鸲 ���ļ�(�����̴� �ӵ�)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float RollAmplitude; //�¿�ȸ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float RollFrequency; //�¿�ȸ�� ���ļ�

	FVector OriginalMeshLocation; //�޽� ������ġ ����
	FRotator OriginalMeshRotation; //�޽� ����ȸ�� ����

	//Post Process Radial Blur ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PoProcess")
	class UMaterialParameterCollection* MPC_DroneBlur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoProcess")
	class UMaterialInterface* RadialBlurMaterial;

	//Post Process Depth Of Field(�ɵ�) ���� ����
	// ����Ʈ ���μ��� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoProcess")
    FPostProcessSettings PostProcessSettings;

    // ���� ���� �Ÿ�
    float FocusDistance;

	//Drone AI Objection ���� ���� Set
	//������ Actors�� �����ϱ� ���� TSet
	TSet<class AKHS_AIVisionObject*> DetectedAIVisionObjects;

	//�±װ� �����Ǿ����� ����
	bool bIsTagSet;

	//�±׸� ����Ͽ� ���� ������ ����
	bool bIsCurrentlyDetecting;

	// SceneCapture2D ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
	class ASceneCapture2D* SceneCaptureActor;

	// ĸó �����͸� ������ �ؽ�ó ���� Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
	class UTextureRenderTarget2D* RenderTarget;

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

	//Post Process(Radial Blur, Depth of Field) ���� �Լ�
	void SetDronePostProcess();

	// VOIP ���� �ʱ�ȭ �۾��� ����
	void InitializeVOIP();

	// ����ũ �Ӱ谪�� ����
	void SetMicThreshold(float Threshold);

	// �÷��̾� ���¿� ���
	void RegisterWithPlayerState();

	// ���� �÷��̾ ���� ������ üũ
	bool IsLocallyControlled() const;

	// ������ ���͸� �ֱ������� Ȯ���ϴ� �Լ�
	//void PeriodicallyCheckVision();

	//�±׸� ���޹޾� Actor�� �˻��� �Լ�
	void CheckVisionForTags(const TArray<FString>& TagsToCheck);

	// �ؽ�ó�� JPEG �̹����� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category="Capture")
	void SaveCaptureToImage();

	// �̹��� ���� ��θ� �����ϴ� �Լ�
	FString GetImagePath(const FString& FileName) const;

	// �̹��� ���� �Լ� (���� ���� ����)
	void SendImageToServer(const FString& ImagePath, const TArray64<uint8>& ImageData);

	// SceneCaptureActor�� ����� ī�޶�� ���� ��ġ �� ������ ����ȭ�ϴ� �Լ�
	void SyncSceneCaptureWithCamera();

};
