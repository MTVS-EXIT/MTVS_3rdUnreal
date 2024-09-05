// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_ServerWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Widget.h"
#include "KJH/KJH_WidgetSystem.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "KJH/KJH_ServerRow.h"
#include "Components/TextBlock.h"
#include "KJH/KJH_GameInstance.h"

 ////////// 생성자 구간 ----------------------------------------------------------------------------------------------------------------------------
UKJH_ServerWidget::UKJH_ServerWidget(const FObjectInitializer& ObjectInitialize)
{

}

bool UKJH_ServerWidget::Initialize()
{
	Super::Initialize();

////////// 메인메뉴 버튼 바인딩 구간 ----------------------------------------------------------------------------------------------------------------
	if (MainMenu_HostButton)
	{
	MainMenu_HostButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::HostServer); // Host 버튼 눌렀을 때 HostServer 함수 호출
	}

	if (MainMenu_JoinButton)
	{
		MainMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::OpenLobbyMenu); // Join 버튼 눌렀을 때 OpenJoinMenu 함수 호출
	}

	if (MainMenu_QuitButton)
	{
		MainMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::QuitPressed); // Quit 버튼 눌렀을 때 QuitPressed 함수 호출
	}

////////// 로비메뉴 버튼 바인딩 구간 ----------------------------------------------------------------------------------------------------------------
	if (LobbyMenu_JoinButton)
	{
		LobbyMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::JoinServer); // Join 버튼 눌렀을 때 JoinServer 함수 호출
	}

	if (LobbyMenu_CancelButton)
	{
		LobbyMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::OpenMainMenu); // Cancel 버튼 눌렀을 때 OpenMainMenu 함수 호출
	}

////////// 캐릭터 선택 버튼 바인딩 구간 -------------------------------------------------------------------------------------------------------------

	if (PersonSelectButton)
	{
		PersonSelectButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::SelectPersonCharacter);
	}

	if (DroneSelectButton)
	{
		DroneSelectButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::SelectDroneCharacter);
	}

	// 캐릭터 선택 버튼 상태 업데이트
	UpdateSelectButtonStates();

	return true;

}

////////// 사용자 정의형 함수 구간 -------------------------------------------------------------------------------------------------
void UKJH_ServerWidget::HostServer()
{
	if (MenuInterface)
	{
		MenuInterface->Host();
	}
}

void UKJH_ServerWidget::SetServerList(TArray<FString> ServerNames)
{
	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FString& ServerName : ServerNames)
	{
	// ServerRowFactory를 통해 ServerRowUI 위젯 생성
	ServerRow = CreateWidget<UKJH_ServerRow>(this, ServerRowFactory);

	// 텍스트의 이름을 설정
	ServerRow->ServerName->SetText(FText::FromString(ServerName));
	ServerRow->Setup(this, i);
	++i;

	// 버튼 누를 시, 서버목록 생성
	ServerList->AddChild(ServerRow);
	}

}

void UKJH_ServerWidget::SelecetIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UKJH_ServerWidget::JoinServer()
{
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		MenuInterface->Join(SelectedIndex.GetValue());
		UE_LOG(LogTemp, Warning, TEXT("Selected Index is %d."), SelectedIndex.GetValue());
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index not set"));
	}
}

void UKJH_ServerWidget::OpenLobbyMenu()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(LobbyMenu); // LobbyMenu로 전환하여 활성화한다.
		UE_LOG(LogTemp, Warning, TEXT("LobbyMenu is Activate"));

		if (MenuInterface)
		{
			MenuInterface->RefreshServerList();
		}
	}
}

void UKJH_ServerWidget::OpenMainMenu()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu); // MainMenu로 전환하여 활성화한다.
	}
}

void UKJH_ServerWidget::QuitPressed()
{
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();

	PlayerController->ConsoleCommand("quit"); // 게임을 종료시킨다.
}

void UKJH_ServerWidget::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		UKJH_ServerRow* Row = Cast<UKJH_ServerRow>(ServerList -> GetChildAt(i));
		if (Row)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue( )== i);
		}
	}
}


////////// 캐릭터 선택 처리 관련 함수 -------------------------------------------------------------------------------------------
void UKJH_ServerWidget::ShowCharacterSelect()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(CharacterSelectMenu); // LobbyMenu로 전환하여 활성화한다.
		UE_LOG(LogTemp, Warning, TEXT("CharacterSelectMenu is Activate"));
	}
}

void UKJH_ServerWidget::SelectPersonCharacter()
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

void UKJH_ServerWidget::SelectDroneCharacter()
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

void UKJH_ServerWidget::UpdateSelectButtonStates()
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
