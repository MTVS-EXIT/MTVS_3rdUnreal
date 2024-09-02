// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_InGameWidget.h"
#include "Components/Button.h"

bool UKJH_InGameWidget::Initialize()
{
	Super::Initialize();


	if (InGameMenu_CancelButton)
	{
		InGameMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::CancelPressed); // Host 버튼 눌렀을 때 HostServer 함수 호출
	}

	if (InGameMenu_QuitButton)
	{
		InGameMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::QuitPressed); // Host 버튼 눌렀을 때 HostServer 함수 호출
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
