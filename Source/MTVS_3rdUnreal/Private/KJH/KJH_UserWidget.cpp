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
	MainMenu_HostButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::HostServer); // Host ��ư ������ �� HostServer �Լ� ȣ��
	}

	if (MainMenu_JoinButton)
	{
		MainMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::OpenLobbyMenu); // Join ��ư ������ �� OpenJoinMenu �Լ� ȣ��
	}

	if (LobbyMenu_CancelButton)
	{
		LobbyMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::OpenMainMenu); // Join ��ư ������ �� OpenMainMenu �Լ� ȣ��
	}

	if (LobbyMenu_JoinButton)
	{
		LobbyMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::JoinServer); // Join ��ư ������ �� JoinServer �Լ� ȣ��
	}

	return true;

}

void UKJH_UserWidget::SetMyInterface(IKJH_Interface* Interface)
{
	this -> MyInterface = Interface;
}

////////// ����� ������ �Լ� ���� -------------------------------------------------------------------------------------------------
void UKJH_UserWidget::HostServer()
{
	if (MyInterface)
	{
		MyInterface->Host();
	}
}

////////// ����� ������ �Լ� ���� -------------------------------------------------------------------------------------------------
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

	// UI �� ��ȿ�ϴٸ�,
	if (this)
	{
		this->AddToViewport(); // Viewport �� UI�� ����
	}

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	check(PlayerController);

	if (PlayerController)
	{
		FInputModeUIOnly InputUIModeData; // UI�� ��ȣ�ۿ��� �� �� �ִ� �Է¸�带 'InputUIModeData'�� �̸����� ����

		InputUIModeData.SetWidgetToFocus(this->TakeWidget()); // ��Ŀ���� ���� ������ ����. ��, ���콺 �Է��� ServerUI ���� ������. �ٸ� ���� Ŭ�� ����.
		InputUIModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // ���콺�� Viewport ȭ�� ���� ��� �� �ִ°�? DoNotLock : ���

		PlayerController->SetInputMode(InputUIModeData); // UI ���� �Է� ��� ����. �÷��̾�� ���� ����� ��ȣ�ۿ� �Ұ��ϰ� UI�ϰ� ��ȣ�ۿ� �� �� �ְ���.
		PlayerController->bShowMouseCursor = true; // ���콺 Ŀ���� ���̰� ��.
	}

}

void UKJH_UserWidget::Teardown() // UI ���� �Լ�
{
	this -> RemoveFromParent(); // Viewport �� UI�� ����

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (PlayerController)
	{
		FInputModeGameOnly InputGameModeData; // Game�� ��ȣ�ۿ��� �� �� �ִ� �Է¸�带 'InputGameModeData'�� �̸����� ����

		PlayerController->SetInputMode(InputGameModeData); // �Է� ��带 ���� ���� ����
		PlayerController->bShowMouseCursor = false; // ���콺 Ŀ���� ���̰� ���� ����.
	}
}

void UKJH_UserWidget::OpenLobbyMenu()
{
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(LobbyMenu); // LobbyMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
		UE_LOG(LogTemp, Warning, TEXT("LobbyMenu is Activate"));
	}
}

void UKJH_UserWidget::OpenMainMenu()
{
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu); // MainMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
	}
}