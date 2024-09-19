// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_CharacterSelectWidget.h"
#include "Delegates/Delegate.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_GameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "KJH/KJH_PlayerController.h"
#include "Engine/World.h"

UKJH_CharacterSelectWidget::UKJH_CharacterSelectWidget(const FObjectInitializer& ObjectInitialize)
{

}

bool UKJH_CharacterSelectWidget::Initialize()
{

	bool Success = Super::Initialize(); // Super::Initialize() ȣ��� �⺻ �ʱ�ȭ�� ���������� �Ǵ��� Ȯ��
	if (!Success) return false;

////////// ĳ���� ���� ��ư ���ε� ���� -------------------------------------------------------------------------------------------------------------

	if (PersonSelectButton)
	{
		PersonSelectButton->OnClicked.AddDynamic(this, &UKJH_CharacterSelectWidget::SelectPersonCharacter);
	}

	if (DroneSelectButton)
	{
		DroneSelectButton->OnClicked.AddDynamic(this, &UKJH_CharacterSelectWidget::SelectDroneCharacter);
	}

	// ĳ���� ���� ��ư ���� ������Ʈ
	UpdateSelectButtonStates();

	return true;
}

////////// ĳ���� ���� ó�� ���� �Լ� -------------------------------------------------------------------------------------------
void UKJH_CharacterSelectWidget::ShowCharacterSelect()
{
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(CharacterSelectMenu); // CharacterSelectMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
		UE_LOG(LogTemp, Warning, TEXT("CharacterSelectMenu is Activate"));
	}
}

void UKJH_CharacterSelectWidget::SelectPersonCharacter()
{
	AKJH_PlayerController* PlayerController = Cast<AKJH_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->bIsPersonCharacterSelected = true;
		PlayerController->ServerSpawnCharacterBasedOnSelection(true);
		UE_LOG(LogTemp, Warning, TEXT("Person Character Selected"));
		UpdateSelectButtonStates();

		// ���� �ִϸ��̼��� ����� ����� �� Teardown
		FTimerHandle TimerHandle_TeardownControl;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_TeardownControl, [this]()
		{
			Teardown();
		}, 2.0f, false);
	}
}

void UKJH_CharacterSelectWidget::SelectDroneCharacter()
{
	AKJH_PlayerController* PlayerController = Cast<AKJH_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->bIsPersonCharacterSelected = false;
		PlayerController->ServerSpawnCharacterBasedOnSelection(false);
		UE_LOG(LogTemp, Warning, TEXT("Drone Character Selected"));
		UpdateSelectButtonStates();

		// ���� �ִϸ��̼��� ����� ����� �� Teardown
		FTimerHandle TimerHandle_TeardownControl;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_TeardownControl, [this]()
		{
			Teardown();
		}, 2.0f, false);
	}
}

void UKJH_CharacterSelectWidget::UpdateSelectButtonStates()
{
	// �÷��̾ � ĳ���͸� �����ߴ��� �˱����� �� ������ ��� �ִ� �����ν��Ͻ��� �����´�.
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// ��ư Ȱ��ȭ/��Ȱ��ȭ ���� ����
		if (GameInstance->bIsPersonSelected)
		{
			PersonSelectButton->SetIsEnabled(false); // ��� �÷��̾� ��ư�� �̹� ���õ� ��� ��Ȱ��ȭ
		}

		if (GameInstance->bIsDroneSelected)
		{
			DroneSelectButton->SetIsEnabled(false); // ��� �÷��̾� ��ư�� �̹� ���õ� ��� ��Ȱ��ȭ
		}
	}
}

void UKJH_CharacterSelectWidget::ShowSpawnWidget()
{
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(CharacterSpawnWidget); // CharacterSpawnWidget�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
		UE_LOG(LogTemp, Warning, TEXT("CharacterSpawnWidget is Activate"));
	}

	if (GetWorld())
	{
		FTimerHandle TimerHandle_TransitionAnimControl;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_TransitionAnimControl, [this]()
		{
			PlayAnimation(ShowSpawnTransitionAnim);
		}, 2.3f, false);
	}
}