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

	bool Success = Super::Initialize(); // Super::Initialize() 호출로 기본 초기화가 정상적으로 되는지 확인
	if (!Success) return false;

////////// 캐릭터 선택 버튼 바인딩 구간 -------------------------------------------------------------------------------------------------------------

	if (PersonSelectButton)
	{
		PersonSelectButton->OnClicked.AddDynamic(this, &UKJH_CharacterSelectWidget::SelectPersonCharacter);
	}

	if (DroneSelectButton)
	{
		DroneSelectButton->OnClicked.AddDynamic(this, &UKJH_CharacterSelectWidget::SelectDroneCharacter);
	}

	// 캐릭터 선택 버튼 상태 업데이트
	UpdateSelectButtonStates();

	return true;
}

////////// 캐릭터 선택 처리 관련 함수 -------------------------------------------------------------------------------------------
void UKJH_CharacterSelectWidget::ShowCharacterSelect()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(CharacterSelectMenu); // CharacterSelectMenu로 전환하여 활성화한다.
		UE_LOG(LogTemp, Warning, TEXT("CharacterSelectMenu is Activate"));
	}
}

void UKJH_CharacterSelectWidget::SelectPersonCharacter()
{
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OnCharacterSelected(true); // Person Player 버튼 선택 시 사람 캐릭터 선택을 게임인스턴스에 알림
		UE_LOG(LogTemp, Warning, TEXT("Person Character Selected"));
		UpdateSelectButtonStates();

		// 캐릭터 선택 후 UI 제거
		Teardown(); // UI가 선택 후 사라지도록 처리
	}
}

void UKJH_CharacterSelectWidget::SelectDroneCharacter()
{
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OnCharacterSelected(false); // Drone Player 버튼 선택 시 드론 캐릭터 선택을 게임인스턴스에 알림
		
		UE_LOG(LogTemp, Warning, TEXT("Drone Character Selected"));
		UpdateSelectButtonStates();

		// 캐릭터 선택 후 UI 제거
		Teardown(); // UI가 선택 후 사라지도록 처리
	}
}

void UKJH_CharacterSelectWidget::UpdateSelectButtonStates()
{
	// 플레이어가 어떤 캐릭터를 선택했는지 알기위해 그 정보를 담고 있는 게임인스턴스를 가져온다.
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// 버튼 활성화/비활성화 설정 구간
		if (GameInstance->bIsPersonSelected)
		{
			PersonSelectButton->SetIsEnabled(false); // 사람 플레이어 버튼이 이미 선택된 경우 비활성화
		}

		if (GameInstance->bIsDroneSelected)
		{
			DroneSelectButton->SetIsEnabled(false); // 드론 플레이어 버튼이 이미 선택된 경우 비활성화
		}
	}
}
