﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputLibrary.h"
#include "Engine/Scene.h"
#include "KHS_AudioCaptureComp.h"
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

	// 새로운 플레이어 컨트롤러 할당되면 Enhanced Input을 다시 매핑해주는 Possessed 함수
	virtual void PossessedBy(AController* NewController) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Function;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Voice;

	//MainUI 인스턴스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DroneMainUIFactory;

	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* DroneMainUI;

	//KHS JsonParseLib 인스턴스
	UPROPERTY(EditDefaultsOnly)
	class UKHS_JsonParseLib* KHSJsonLib;

	//AI HUD UI 설정 변수 Set

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PoProcess")
	class UMaterialParameterCollection* MPC_DroneBlur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoProcess")
	class UMaterialInterface* RadialBlurMaterial;

	//Post Process Depth Of Field(심도) 결정 변수
	// 포스트 프로세스 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoProcess")
    FPostProcessSettings PostProcessSettings;

    // 현재 초점 거리
    float FocusDistance;

	//Drone AI Objection 설정 변수 Set
	//감지된 Actors를 추적하기 위한 TSet
	TSet<class AKHS_AIVisionObject*> DetectedAIVisionObjects;

	//태그가 설정되었는지 여부
	bool bIsTagSet;

	//태그를 사용하여 감지 중인지 여부
	bool bIsCurrentlyDetecting;

	//AI Image Sending URL
	//FString AIDetectionURL = "metaai.iptime.org:7722/detect/";
	FString AIDetectionURL = "http://192.168.219.116:7722/detect/";

	// SceneCapture2D 액터 참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
	class ASceneCapture2D* SceneCaptureActor;

	// 캡처 데이터를 저장할 텍스처 렌더 타겟
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
	class UTextureRenderTarget2D* RenderTarget;

	//AIChatBot Sending URL
	//FString AIChatbotURL = "metaai.iptime.org:7722/chatbot/";
	FString AIChatbotURL = "http://192.168.219.116:7722/chatbot/";

	//AIChatBot STT 변수들
	FString DisplayedText; //현재까지 표시된 텍스트들
	FString FullText; //파싱한 전체 텍스트
	int32 CurrentCharIndex; //표시텍스트 인덱스
	FTimerHandle TextDisplayTimerHandle; //텍스트 애니메이션 핸들

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

	//고도계 업데이트 함수
	void DroneAltitudeUpdate();

	//Post Process(Radial Blur, Depth of Field) 설정 함수
	void SetDronePostProcess();

	//Drone Outline PostProcess 효과 함수
	void DroneShowOutline();

	// 라인트레이스 기반 윤곽선 표시 함수
	void DroneEnableOutline(AActor* HitActor);

	// 라인트레이스 기반 시야 벗어났을때 윤곽선 해제 함수
	void DroneDisableOutline(AActor* HitActor);

	// VOIP 관련 초기화 작업을 수행
	void InitializeVOIP();

	// 마이크 임계값을 설정
	void SetMicThreshold(float Threshold);

	// 플레이어 상태에 등록
	void RegisterWithPlayerState();

	// 로컬 플레이어가 제어 중인지 체크
	bool IsLocallyControlled() const;

	//StartNetworkVoice 네트워크로 사운드를 보냄
	void SetUpNetworkVoice(); 

	//StopNetworkVoice 네트워크로 사운드 보내기 중지
	void StopVoice(); 

	//VOIP 대상자 등록
	void RegisterRemoteTalker();

	// 감지된 액터를 주기적으로 확인하는 함수
	//void PeriodicallyCheckVision();

	//태그를 전달받아 Actor를 검사할 함수
	void CheckVisionForTags(const TArray<FString>& TagsToCheck);

	// 텍스처를 JPEG 이미지로 저장하는 함수
	UFUNCTION(BlueprintCallable, Category="Capture")
	void SaveCaptureToImage();

	// 이미지 저장 경로를 설정하는 함수
	FString GetImagePath(const FString& FileName) const;

	// 이미지 전송 함수 (서버 전송 구현)
	void SendImageToServer(const FString& ImagePath, const TArray64<uint8>& ImageData);

	// AI에게 처리 이미지를 반환받을때 처리를 진행할 함수
	void OnResGetAIImage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// SceneCaptureActor를 드론의 카메라와 같은 위치 및 각도로 동기화하는 함수
	void SyncSceneCaptureWithCamera();

	// 서버로 오디오 파일 전송 함수
	UFUNCTION(BlueprintCallable)
	void SendAudioToServer(const FString& FilePath);

	// 서버로 오디오 파일 업로드 완료 시 호출되는 콜백 함수
	void OnAudioUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// STT 콜백 함수
	void CallParsingAIText(const FString& json);
	// Text 애니메이션 함수
	void UpdateDisplayedText();

	// STS 콜백 함수
	void CallParsingAISound(const FString& json);

	UFUNCTION(BlueprintCallable)
	void TestSound();

};
