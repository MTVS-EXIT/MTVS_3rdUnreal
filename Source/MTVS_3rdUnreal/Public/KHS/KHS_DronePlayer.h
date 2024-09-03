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

	//충돌체, 메시, 카메라
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCameraComponent* CameraComp;

	// VOIP Talker 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice Chat", meta = (AllowPrivateAccess = "true"))
	class UVOIPTalker* VOIPTalkerComp;

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
    class UInputAction* IA_DroneLook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* IA_DroneFwd;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneRight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneUp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_DroneDown;

	//MainUI 인스턴스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DroneMainUIFactory;

	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* DroneMainUI;

	//카메라쉐이크 인스턴스(움직임)
	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> DroneCameraShake;
	//쉐이크 주기
	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	float DroneShakeInterval;
	//마지막 쉐이크 후 경과시간
	float TimeSinceLastShake;

	//Hovering 메시 흔들림 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float HoverAmplitude; //상하흔들림 진폭(메시가 움직이는 범위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float HoverFrequency; //상하흔들림 주파수(움직이는 속도)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float RollAmplitude; //좌우회전 진폭
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovering")
	float RollFrequency; //좌우회전 주파수

	FVector OriginalMeshLocation; //메시 기존위치 저장
	FRotator OriginalMeshRotation; //메시 기존회전 저장

	//Post Process Radial Blur 강도 결정 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PostProcess")
	class UMaterialParameterCollection* MPC_DroneBlur;

	//감지된 Actors를 추적하기 위한 TSet
	TSet<class AKHS_AIVisionObject*> DetectedAIVisionObjects;

	//현재 감지할 태그 저장변수
	FString CurrentTag;

	//태그가 설정되었는지 여부
	bool bIsTagSet;

	//태그를 사용하여 감지 중인지 여부
	bool bIsCurrentlyDetecting;

	//==============================================
	//함수
	//==============================================

	//드론 이동함수
	void DroneLook(const FInputActionValue& Value);
	void DroneMoveFwd(const FInputActionValue& Value);
	void DroneMoveRight(const FInputActionValue& Value);
	void DroneMoveUp(const FInputActionValue& Value);
	void DroneMoveDown(const FInputActionValue& Value);

	//카메라쉐이크 재생함수
	void PlayDroneCameraShake();

	// VOIP 관련 초기화 작업을 수행
	void InitializeVOIP();

	// 마이크 임계값을 설정
	void SetMicThreshold(float Threshold);

	// 플레이어 상태에 등록
	void RegisterWithPlayerState();

	// 로컬 플레이어가 제어 중인지 체크
	bool IsLocallyControlled() const;

	// 감지된 액터를 주기적으로 확인하는 함수
	//void PeriodicallyCheckVision();

	//태그를 전달받아 Actor를 검사할 함수
	void CheckVisionForTag(FString Tag);
};
