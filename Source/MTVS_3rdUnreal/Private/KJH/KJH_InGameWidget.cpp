// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_InGameWidget.h"
#include "Components/Button.h"

bool UKJH_InGameWidget::Initialize()
{
	Super::Initialize();


	if (InGameMenu_CancelButton)
	InGameMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::CancelPressed); // Cancel ��ư ������ �� CancelPressed �Լ� ȣ��

	if (InGameMenu_QuitButton)
	InGameMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::QuitPressed); // Quit ��ư ������ �� QuitPressed �Լ� ȣ��

	return true;
}

void UKJH_InGameWidget::Setup()
{
	Super::Setup();

	if(InGameMenuAnim)
	PlayAnimation(InGameMenuAnim);
}

////////// ����� ������ �Լ� ���� ============================================================================================================
// �ΰ��� Widget -> �������� �����ϴ� ��ư
void UKJH_InGameWidget::CancelPressed()
{
	Teardown(); // Widget �ı�
}

// �ΰ��� Widget -> ������ �����Ͽ� ���θ޴��� �̵��ϴ� �Լ�
void UKJH_InGameWidget::QuitPressed()
{
	if (MenuInterface)
	{
		MenuInterface->LoadServerWidgetMap(); // ServerWidget������ �̵�
		Teardown(); // Widget �ı�
	}
}
