// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_CharacterSelectWidget.h"
#include "Delegates/Delegate.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_GameInstance.h"
#include "Components/WidgetSwitcher.h"

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
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		APlayerController* PlayerController = GetOwningPlayer(); // ���� ������ ������ ��Ʈ�ѷ� ��������
		GameInstance->OnCharacterSelected(PlayerController, true); // Person Player ��ư ���� �� ��Ʈ�ѷ��� ���� ���� ����

		UE_LOG(LogTemp, Warning, TEXT("Person Character Selected"));
		UpdateSelectButtonStates();


		Teardown(); // ĳ���� ���� �� UI ����
	}
}

void UKJH_CharacterSelectWidget::SelectDroneCharacter()
{
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		APlayerController* PlayerController = GetOwningPlayer(); // ���� ������ ������ ��Ʈ�ѷ� ��������
		GameInstance->OnCharacterSelected(PlayerController, false); // Drone Player ��ư ���� �� ��Ʈ�ѷ��� ���� ���� ����
		
		UE_LOG(LogTemp, Warning, TEXT("Drone Character Selected"));
		UpdateSelectButtonStates();

		Teardown(); // ĳ���� ���� �� UI ����
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
