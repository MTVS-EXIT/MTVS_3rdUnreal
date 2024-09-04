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

 ////////// ������ ���� ----------------------------------------------------------------------------------------------------------------------------
UKJH_ServerWidget::UKJH_ServerWidget(const FObjectInitializer& ObjectInitialize)
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

////////// ĳ���� ���� ��ư ���ε� ���� -------------------------------------------------------------------------------------------------------------

	if (PersonSelectButton)
	{
		PersonSelectButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::SelectPersonCharacter);
	}

	if (DroneSelectButton)
	{
		DroneSelectButton->OnClicked.AddDynamic(this, &UKJH_ServerWidget::SelectDroneCharacter);
	}

	// ĳ���� ���� ��ư ���� ������Ʈ
	UpdateSelectButtonStates();

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

void UKJH_ServerWidget::SetServerList(TArray<FString> ServerNames)
{
	if (!ServerList)
	{
		UE_LOG(LogTemp, Error, TEXT("SetServerList failed: ServerList is not initialized."));
		return;
	}

	ServerList->ClearChildren();

	if (!ServerRowFactory)
	{
		UE_LOG(LogTemp, Error, TEXT("SetServerList failed: ServerRowFactory is not valid."));
		return;
	}

	uint32 i = 0;
	for (const FString& ServerName : ServerNames)
	{
		UKJH_ServerRow* NewServerRow = CreateWidget<UKJH_ServerRow>(this, ServerRowFactory);
		if (!NewServerRow)
		{
			UE_LOG(LogTemp, Error, TEXT("SetServerList failed: Failed to create ServerRow widget."));
			continue;
		}

		// ���� �̸��� ������ �� �ִ��� Ȯ��
		if (NewServerRow->ServerName)
		{
			NewServerRow->ServerName->SetText(FText::FromString(ServerName));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SetServerList failed: ServerRow's ServerName TextBlock is not valid."));
		}

		NewServerRow->Setup(this, i);
		++i;

		// ServerRow�� ServerList�� �߰��ϱ� ���� ��ȿ�� Ȯ��
		if (ServerList)
		{
			ServerList->AddChild(NewServerRow);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SetServerList failed: ServerList is not valid when adding ServerRow."));
		}
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
	// WidgetSwitcher Ÿ���� MenuSwitcher�� ������
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(LobbyMenu); // LobbyMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
		UE_LOG(LogTemp, Warning, TEXT("LobbyMenu is Activate"));

		if (MenuInterface)
		{
			MenuInterface->RefreshServerList();
		}
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


////////// ĳ���� ���� ó�� ���� �Լ� -------------------------------------------------------------------------------------------
void UKJH_ServerWidget::ShowCharacterSelect()
{
	// MenuSwitcher�� CharacterSelectMenu�� ��ȿ���� Ȯ��
	if (!MenuSwitcher || !CharacterSelectMenu)
	{
		UE_LOG(LogTemp, Error, TEXT("ShowCharacterSelect failed: MenuSwitcher or CharacterSelectMenu is not valid."));
		return;
	}

		MenuSwitcher->SetActiveWidget(CharacterSelectMenu); // LobbyMenu�� ��ȯ�Ͽ� Ȱ��ȭ�Ѵ�.
		UE_LOG(LogTemp, Warning, TEXT("CharacterSelectMenu is Activate"));

}

void UKJH_ServerWidget::SelectPersonCharacter()
{
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OnCharacterSelected(true); // Person Player ��ư ���� �� ��� ĳ���� ������ �����ν��Ͻ��� �˸�
		UE_LOG(LogTemp, Warning, TEXT("Person Character Selected"));
		UpdateSelectButtonStates();


		// ĳ���� ���� �� UI ����
		Teardown(); // UI�� ���� �� ��������� ó��
	}
}

void UKJH_ServerWidget::SelectDroneCharacter()
{
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OnCharacterSelected(false); // Drone Player ��ư ���� �� ��� ĳ���� ������ �����ν��Ͻ��� �˸�
		UE_LOG(LogTemp, Warning, TEXT("Drone Character Selected"));
		UpdateSelectButtonStates();


		// ĳ���� ���� �� UI ����
		Teardown(); // UI�� ���� �� ��������� ó��
	}
}

void UKJH_ServerWidget::UpdateSelectButtonStates()
{
	// �÷��̾ � ĳ���͸� �����ߴ��� �˱����� �� ������ ��� �ִ� �����ν��Ͻ��� �����´�.
	GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// ��ư Ȱ��ȭ/��Ȱ��ȭ ���� ����
		if (GameInstance->bIsPersonSelected)
		{
			PersonSelectButton->SetIsEnabled(false); // ��� �÷��̾� ��ư�� �̹� ���õ� ��� ��Ȱ��ȭ
		}

		if (GameInstance->bIsDroneSelected)
		{
			DroneSelectButton->SetIsEnabled(false); // ��� �÷��̾� ��ư�� �̹� ���õ� ��� ��Ȱ��ȭ
		}
	}
}
