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

	// ������ ����
	UKJH_CharacterSelectWidget(const FObjectInitializer& ObjectInitialize);

	// Initialization ����
	virtual bool Initialize() override;

////////// UI ���ε� ���� ------------------------------------------------------------------------------------------------------

	// �޴� ü���� ���� //
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI�� ü���� ��ų �� �ִ� Switcher

	// ĳ���� ���� UI ����
	UPROPERTY(meta = (BindWidget))
	class UWidget* CharacterSelectMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* PersonSelectButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* DroneSelectButton;

////////// ���� ���� ���� -----------------------------------------------------------------------------------------------------
	class UKJH_GameInstance* GameInstance; // GameInstance ���� ����


////////// ����� ������ �Լ� ���� --------------------------------------------------------------------------------------------
	UFUNCTION()
    void ShowCharacterSelect();  // ĳ���� ���� UI�� ǥ���ϴ� �Լ�

	UFUNCTION()
    void SelectPersonCharacter(); // ��� ĳ���� ���� ó�� �Լ�

	UFUNCTION()
    void SelectDroneCharacter(); // ��� ĳ���� ���� ó�� �Լ�

	void UpdateSelectButtonStates(); // ���õ� ��ư ���¸� Ȯ���ϰ� �� ���ο� ���� ������Ʈ�ϴ� �Լ�
};
