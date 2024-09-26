// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_CharacterSelectWidget.h"
#include "Delegates/Delegate.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_GameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "KJH/KJH_PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "KJH/KJH_PlayerState.h"

UKJH_CharacterSelectWidget::UKJH_CharacterSelectWidget(const FObjectInitializer& ObjectInitialize)
{

}

bool UKJH_CharacterSelectWidget::Initialize()
{

	bool Success = Super::Initialize(); // Super::Initialize() 호출로 기본 초기화가 정상적으로 되는지 확인
	if (!Success) return false;

////////// 캐릭터 선택 버튼 바인딩 구간 -------------------------------------------------------------------------------------------------------------

	if (PersonSelectButton)
		PersonSelectButton->OnClicked.AddDynamic(this, &UKJH_CharacterSelectWidget::SelectPersonCharacter);

	if (DroneSelectButton)
		DroneSelectButton->OnClicked.AddDynamic(this, &UKJH_CharacterSelectWidget::SelectDroneCharacter);

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
	AKJH_PlayerController* PlayerController = Cast<AKJH_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		// PlayerState 업데이트
		AKJH_PlayerState* PS = PlayerController->GetPlayerState<AKJH_PlayerState>();
		if (PS)
		{
			PS->Server_SetIsPersonCharacter(true);
			UE_LOG(LogTemp, Warning, TEXT("Updating PlayerState: Person Character Selected"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerState"));
		}

		// PlayerController 업데이트 및 서버에 알림
		PlayerController->Server_UpdateCharacterSelection(true);

		// 캐릭터 스폰 요청
		PlayerController->ServerSpawnCharacterBasedOnSelection(true);

		UE_LOG(LogTemp, Warning, TEXT("Person Character Selected and Spawn Requested"));

		UpdateSelectButtonStates();

		// 스폰 애니메이션이 충분히 재생된 후 Teardown
		FTimerHandle TimerHandle_TeardownControl;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_TeardownControl, [this]()
		{
			Teardown();
			UE_LOG(LogTemp, Warning, TEXT("Character Select Widget Torn Down"));
		}, 2.0f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController"));
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

		// 스폰 애니메이션이 충분히 재생된 후 Teardown
		FTimerHandle TimerHandle_TeardownControl;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_TeardownControl, [this]()
		{
			Teardown();
		}, 2.0f, false);
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
			PersonSelectButton->SetIsEnabled(false); // 사람 플레이어 버튼이 이미 선택된 경우 비활성화

		if (GameInstance->bIsDroneSelected)
			DroneSelectButton->SetIsEnabled(false); // 드론 플레이어 버튼이 이미 선택된 경우 비활성화
	}
}

void UKJH_CharacterSelectWidget::ShowSpawnWidget()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(CharacterSpawnWidget); // CharacterSpawnWidget로 전환하여 활성화한다.
		UE_LOG(LogTemp, Warning, TEXT("CharacterSpawnWidget is Activate"));
	}

	if (GetWorld())
	{
		FTimerHandle TimerHandle_TransitionAnimControl;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_TransitionAnimControl, [this]()
		{
			PlayAnimation(ShowSpawnTransitionAnim);
			UGameplayStatics::PlaySound2D(this, SpawnNoiseSound, 1.0f, 1.0f, 0.0f);
		}, 2.3f, false);
	}
}