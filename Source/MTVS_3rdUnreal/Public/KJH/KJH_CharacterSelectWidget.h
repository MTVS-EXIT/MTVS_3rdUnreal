// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_WidgetSystem.h"
#include "KJH_Interface.h"
#include "KJH_CharacterSelectWidget.generated.h"

/**
 *
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_CharacterSelectWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()

public:

////////// 생성자 & 초기화 함수 구간 ===========================================================================================
	UKJH_CharacterSelectWidget(const FObjectInitializer& ObjectInitialize); // 생성자 선언
	virtual bool Initialize() override; // Initialization 선언

////////// UI 바인딩 구간 =======================================================================================================
	// 1) 메뉴 체인지 관련 ------------------------------------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI를 체인지 시킬 수 있는 Switcher
	
	// 2) 캐릭터 선택 UI 관련 ---------------------------------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* CharacterSelectMenu; // 캐릭터 선택 Widget UI

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* PersonSelectButton; // 사람 선택 버튼

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* DroneSelectButton; // 드론 선택 버튼

	// 3) 스폰 UI 관련 ----------------------------------------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* CharacterSpawnWidget; // 캐릭터 스폰 시 Glitch Widget UI

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowSpawnTransitionAnim; // 캐릭터 스폰 시 화면 밝아지는 Widget UI

////////// TSubclass & class 참조 구간 ============================================================================================
	// 1) 게임인스턴스 관련 참조 --------------------------------------------------------------------------------------------------
	class UKJH_GameInstance* GameInstance; // GameInstance 참조 선언

	// 2) 사운드 관련 참조 --------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundWave* SpawnNoiseSound; // 스폰 시 애니메이션과 함께 재생되는 Noise 사운드 참조

////////// 사용자 정의형 함수 구간 - 캐릭터 선택 관련 =============================================================================
	UFUNCTION()
	void ShowCharacterSelect();  // 캐릭터 선택 UI를 표시하는 함수

	UFUNCTION()
	void SelectPersonCharacter(); // 사람 캐릭터 선택 처리 함수

	UFUNCTION()
	void SelectDroneCharacter(); // 드론 캐릭터 선택 처리 함수

	void UpdateSelectButtonStates(); // 선택된 버튼 상태를 확인하고 그 여부에 따라 업데이트하는 함수

////////// 사용자 정의형 함수 구간 - 캐릭터 스폰 애니메이션 관련 =============================================================================
	void ShowSpawnWidget();
};
