// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_InGameWidget.h"
#include "Components/Button.h"

bool UKJH_InGameWidget::Initialize()
{
	Super::Initialize();


	if (InGameMenu_CancelButton)
	{
		InGameMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::CancelPressed); // Cancel 버튼 눌렀을 때 CancelPressed 함수 호출
	}

	if (InGameMenu_QuitButton)
	{
		InGameMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::QuitPressed); // Quit 버튼 눌렀을 때 QuitPressed 함수 호출
	}

	return true;
}

void UKJH_InGameWidget::CancelPressed()
{
	Teardown();
}

void UKJH_InGameWidget::QuitPressed()
{
	if (MenuInterface)
	{
		Teardown();
		MenuInterface->LoadServerMenuMap();
	}
}
