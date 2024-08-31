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

////////// ���θ޴� ��ư ���ε� ���� ----------------------------------------------------------------------------------------------------------------
	if (MainMenu_HostButton)
	{
	MainMenu_HostButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::HostServer); // Host ��ư ������ �� HostServer �Լ� ȣ��
	}

	if (MainMenu_JoinButton)
	{
		MainMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::OpenLobbyMenu); // Join ��ư ������ �� OpenJoinMenu �Լ� ȣ��
	}

	if (MainMenu_QuitButton)
	{
		MainMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::QuitPressed); // Quit ��ư ������ �� QuitPressed �Լ� ȣ��
	}

////////// �κ�޴� ��ư ���ε� ���� ----------------------------------------------------------------------------------------------------------------
	if (LobbyMenu_JoinButton)
	{
		LobbyMenu_JoinButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::JoinServer); // Join ��ư ������ �� JoinServer �Լ� ȣ��
	}

	if (LobbyMenu_CancelButton)
	{
		LobbyMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::OpenMainMenu); // Cancel ��ư ������ �� OpenMainMenu �Լ� ȣ��
	}



	return true;

}

////////// ����� ������ �Լ� ���� -------------------------------------------------------------------------------------------------
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
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(LobbyMenu); // LobbyMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
		UE_LOG(LogTemp, Warning, TEXT("LobbyMenu is Activate"));
	}
}

void UKJH_ServerWidget::OpenMainMenu()
{
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu); // MainMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
	}
}

void UKJH_ServerWidget::QuitPressed()
{
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();

	PlayerController->ConsoleCommand("quit"); // ������ �����Ų��.
}
