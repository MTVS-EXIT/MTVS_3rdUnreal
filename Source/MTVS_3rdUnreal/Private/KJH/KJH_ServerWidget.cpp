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

	return true;
}

////////// 사용자 정의형 함수 구간 -------------------------------------------------------------------------------------------------
void UKJH_ServerWidget::HostServer()
{
	if (MenuInterface)
	{
		MenuInterface->Host("Hello");
	}
}

void UKJH_ServerWidget::SetServerList(TArray<FServerData> ServerNames)
{
	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
	// ServerRowFactory를 통해 ServerRowUI 위젯 생성
	ServerRow = CreateWidget<UKJH_ServerRow>(this, ServerRowFactory);

	// 텍스트의 이름을 설정
	ServerRow->ServerName->SetText(FText::FromString(ServerData.Name));
	ServerRow->HostUser->SetText(FText::FromString(ServerData.HostUserName));
	ServerRow->ConnectedPlayer->SetText(FText::FromString
								(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers)));
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