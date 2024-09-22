// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_InGameWidget.h"
#include "Components/Button.h"

bool UKJH_InGameWidget::Initialize()
{
	Super::Initialize();


	if (InGameMenu_CancelButton)
	InGameMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::CancelPressed); // Cancel 버튼 눌렀을 때 CancelPressed 함수 호출

	if (InGameMenu_QuitButton)
	InGameMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_InGameWidget::QuitPressed); // Quit 버튼 눌렀을 때 QuitPressed 함수 호출

	return true;
}

void UKJH_InGameWidget::Setup()
{
	Super::Setup();

	if(InGameMenuAnim)
	PlayAnimation(InGameMenuAnim);
}

////////// 사용자 정의형 함수 구간 ============================================================================================================
// 인게임 Widget -> 게임으로 복귀하는 버튼
void UKJH_InGameWidget::CancelPressed()
{
	Teardown(); // Widget 파괴
}

// 인게임 Widget -> 게임을 종료하여 메인메뉴로 이동하는 함수
void UKJH_InGameWidget::QuitPressed()
{
	if (MenuInterface)
	{
		MenuInterface->LoadServerWidgetMap(); // ServerWidget맵으로 이동
		Teardown(); // Widget 파괴
	}
}
