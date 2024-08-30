// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_UserWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Widget.h"
#include "Components/EditableTextBox.h"

void UKJH_UserWidget::NativeConstruct()
{

}

bool UKJH_UserWidget::Initialize()
{
	Super::Initialize();

	if (MainMenu_HostButton)
	{
	MainMenu_HostButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::HostServer); // Host 버튼 눌렀을 때 HostServer 함수 호출
	}

	if (MainMenu_JoinButton)
	{
		MainMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::OpenLobbyMenu); // Join 버튼 눌렀을 때 OpenJoinMenu 함수 호출
	}

	if (LobbyMenu_CancelButton)
	{
		LobbyMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::OpenMainMenu); // Join 버튼 눌렀을 때 OpenMainMenu 함수 호출
	}

	if (LobbyMenu_JoinButton)
	{
		LobbyMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::JoinServer); // Join 버튼 눌렀을 때 JoinServer 함수 호출
	}

	return true;

}

void UKJH_UserWidget::SetMyInterface(IKJH_Interface* Interface)
{
	this -> MyInterface = Interface;
}

////////// 사용자 정의형 함수 구간 -------------------------------------------------------------------------------------------------
void UKJH_UserWidget::HostServer()
{
	if (MyInterface)
	{
		MyInterface->Host();
	}
}

////////// 사용자 정의형 함수 구간 -------------------------------------------------------------------------------------------------
void UKJH_UserWidget::JoinServer()
{
	if (MyInterface)
	{
		if (IPAddressField)
		{
			const FString& Address = IPAddressField->GetText().ToString();
			MyInterface->Join(Address);
		}
	}
}


void UKJH_UserWidget::Setup()
{

	// UI 가 유효하다면,
	if (this)
	{
		this->AddToViewport(); // Viewport 상에 UI를 노출
	}

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	check(PlayerController);

	if (PlayerController)
	{
		FInputModeUIOnly InputUIModeData; // UI와 상호작용을 할 수 있는 입력모드를 'InputUIModeData'란 이름으로 설정

		InputUIModeData.SetWidgetToFocus(this->TakeWidget()); // 포커스를 받을 위젯을 설정. 즉, 마우스 입력은 ServerUI 에만 가능함. 다른 곳은 클릭 막음.
		InputUIModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스가 Viewport 화면 밖을 벗어날 수 있는가? DoNotLock : 허용

		PlayerController->SetInputMode(InputUIModeData); // UI 전용 입력 모드 적용. 플레이어는 게임 월드와 상호작용 불가하고 UI하고만 상호작용 할 수 있게함.
		PlayerController->bShowMouseCursor = true; // 마우스 커서를 보이게 함.
	}

}

void UKJH_UserWidget::Teardown() // UI 제거 함수
{
	this -> RemoveFromParent(); // Viewport 상에 UI를 제거

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (PlayerController)
	{
		FInputModeGameOnly InputGameModeData; // Game과 상호작용을 할 수 있는 입력모드를 'InputGameModeData'란 이름으로 설정

		PlayerController->SetInputMode(InputGameModeData); // 입력 모드를 게임 모드로 설정
		PlayerController->bShowMouseCursor = false; // 마우스 커서를 보이게 하지 않음.
	}
}

void UKJH_UserWidget::OpenLobbyMenu()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(LobbyMenu); // LobbyMenu로 전환하여 활성화한다.
		UE_LOG(LogTemp, Warning, TEXT("LobbyMenu is Activate"));
	}
}

void UKJH_UserWidget::OpenMainMenu()
{
	// WidgetSwitcher 타입인 MenuSwitcher가 있으면
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu); // MainMenu로 전환하여 활성화한다.
	}
}