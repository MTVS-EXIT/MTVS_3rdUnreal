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
	
public :

	// 생성자 선언
	UKJH_CharacterSelectWidget(const FObjectInitializer& ObjectInitialize);

	// Initialization 선언
	virtual bool Initialize() override;

////////// UI 바인딩 구간 ------------------------------------------------------------------------------------------------------

	// 메뉴 체인지 관련 //
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI를 체인지 시킬 수 있는 Switcher

	// 캐릭터 선택 UI 관련
	UPROPERTY(meta = (BindWidget))
	class UWidget* CharacterSelectMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* PersonSelectButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* DroneSelectButton;

////////// 변수 참조 구간 -----------------------------------------------------------------------------------------------------
	class UKJH_GameInstance* GameInstance; // GameInstance 참조 선언


////////// 사용자 정의형 함수 구간 --------------------------------------------------------------------------------------------
	UFUNCTION()
    void ShowCharacterSelect();  // 캐릭터 선택 UI를 표시하는 함수

	UFUNCTION()
    void SelectPersonCharacter(); // 사람 캐릭터 선택 처리 함수

	UFUNCTION()
    void SelectDroneCharacter(); // 사람 캐릭터 선택 처리 함수

	void UpdateSelectButtonStates(); // 선택된 버튼 상태를 확인하고 그 여부에 따라 업데이트하는 함수
};
