// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_ServerWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Widget.h"
#include "KJH/KJH_WidgetSystem.h"
#include "Components/EditableTextBox.h"

void UKJH_ServerWidget::NativeConstruct()
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
		MenuInterface->Host();
	}
}

void UKJH_ServerWidget::JoinServer()
{
	if (MenuInterface)
	{
		if (IPAddressField)
		{
			const FString& Address = IPAddressField->GetText().ToString();
			MenuInterface->Join(Address);
		}
	}
}

void UKJH_ServerWidget::OpenLobbyMenu()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(LobbyMenu); // LobbyMenu로 전환하여 활성화한다.
		UE_LOG(LogTemp, Warning, TEXT("LobbyMenu is Activate"));
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
