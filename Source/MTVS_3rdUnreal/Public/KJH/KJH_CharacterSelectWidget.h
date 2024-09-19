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

////////// ������ & �ʱ�ȭ �Լ� ���� ===========================================================================================
	UKJH_CharacterSelectWidget(const FObjectInitializer& ObjectInitialize); // ������ ����

	virtual bool Initialize() override; // Initialization ����

////////// UI ���ε� ���� =======================================================================================================

	// 1) �޴� ü���� ���� ---------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI�� ü���� ��ų �� �ִ� Switcher

	// 2) ĳ���� ���� UI ���� ------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* CharacterSelectMenu; // ĳ���� ���� Widget UI

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* PersonSelectButton; // ��� ���� ��ư
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* DroneSelectButton; // ��� ���� ��ư

	// 3) ���� UI ���� ------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* CharacterSpawnWidget; // ĳ���� ���� �� Glitch Widget UI

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowSpawnTransitionAnim; // ĳ���� ���� �� ȭ�� ������� Widget UI

////////// TSubclass & class ���� ���� ============================================================================================
	class UKJH_GameInstance* GameInstance; // GameInstance ���� ����


////////// ����� ������ �Լ� ���� - ĳ���� ���� ���� =============================================================================
	UFUNCTION()
    void ShowCharacterSelect();  // ĳ���� ���� UI�� ǥ���ϴ� �Լ�

	UFUNCTION()
    void SelectPersonCharacter(); // ��� ĳ���� ���� ó�� �Լ�

	UFUNCTION()
    void SelectDroneCharacter(); // ��� ĳ���� ���� ó�� �Լ�

	void UpdateSelectButtonStates(); // ���õ� ��ư ���¸� Ȯ���ϰ� �� ���ο� ���� ������Ʈ�ϴ� �Լ�

////////// ����� ������ �Լ� ���� - ĳ���� ���� �ִϸ��̼� ���� =============================================================================
	void ShowSpawnWidget();
};
