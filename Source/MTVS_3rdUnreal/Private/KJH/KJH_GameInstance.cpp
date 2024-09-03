// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_Interface.h"
#include "KJH/KJH_ServerWidget.h"
#include "KJH/KJH_InGameWidget.h"
#include "KJH/KJH_WidgetSystem.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"
#include "JSH/JSH_Player.h"
#include "KHS/KHS_DronePlayer.h"
#include "KJH/KJH_PlayerState.h"
#include "KJH/KJH_GameModeBase.h"

// ���� ������ ����� �� �ִ� ���� �̸��� ���� ����� ����
const static FName SESSION_NAME = TEXT("EXIT Session Game");

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // ������ ������ �� �����ϴ� ������.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // �÷��̸� ������ ���� �����ϴ� ������. �ʱ�ȭ�� �����ش�.
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(); // OnlineSubsystem ��������
	if (Subsystem) // ����, Subsystem�� ��ȿ�ϴٸ�,
	{
		SessionInterface = Subsystem->GetSessionInterface(); // ���� �������̽� ��������
		
		// ����, ���� �������̽��� ��ȿ�ϴٸ�,
		if (SessionInterface.IsValid())
		{									// 1���� ���� ���, CreateSession�� �������� �޾Ƽ� OnCreateSessionComplete �Լ� �����Ѵٴ� ��. 
											// (Enhanced Input ó�� ���ε��Ѵٰ� �����ϸ� ����. �������� ����ϰ� ���ε�.
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnJoinSessionComplete);
		}
	}
}

////////// ��������Ʈ ���ε� �Լ� ���� ���� ------------------------------------------------------------------------------
void UKJH_GameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	// ���� ������ ������ listen ������ �����ش�.
	GetWorld()->ServerTravel("/Game/MAPS/KJH/KJH_TestMap?listen");

	// ������ ���������� ������ �Ŀ��� UI�� �����ϴ� Teardown �Լ��� �����Ѵ�.
	if (ServerWidget)
	{
		ServerWidget->Teardown();
	}
}


// ���� �ı� �Ϸ� �� ȣ��Ǵ� �Լ�
void UKJH_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) // �ı��� �����ϸ�?
	{
		CreateSession(); // ������ ����������. (������ ������ ������ �װ��� �μ��� ���ο� ������ ����ٴ� ���̴�.)
	}
}


void UKJH_GameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true; // LAN ��� ����, true �� LAN ������ ã�� false �� ���ͳ� ������ ã��.
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		// ToSharedRef -> TSharedPtr �� �׻� ��ȿ�ϰ� �ٲ��ִ� �޼���. TSharedptr �� Null�� ���� �ִµ�, 
		// FindSession�̶� �޼���� Null�̸� �����ϴϱ� ���ʿ� ��ȿ�� �༮�� �ְ� ��û�Ѵ�. 
		// �׷��� �츮�� �׻� ��ȿ�ϰ� ToSharedRef�� ��ȯ������Ѵ�.
	}
}

void UKJH_GameInstance::OnFindSessionComplete( bool Success)
{

	if (Success && SessionSearch.IsValid() && ServerWidget != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));

		TArray<FString> ServerNames;
		//ServerNames.Add("Test Servr1"); // �׽�Ʈ �ؽ�Ʈ�� ����
		//ServerNames.Add("Test Servr2");
		//ServerNames.Add("Test Servr3");

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session Names S: %s"), *SearchResult.GetSessionIdStr());
			ServerNames.Add(SearchResult.GetSessionIdStr());
		}

		ServerWidget -> SetServerList(ServerNames);
	}
}

// ���ǿ� ���� ��� ȣ��Ǵ� �Լ�
void UKJH_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{

	FString Address;

	if (SessionInterface.IsValid())
	{
		if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			UE_LOG(LogTemp, Warning, TEXT("Could Not Get Connect String"));
			return;
		}
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
													// �ش� �ɼ��� ���� ��θ� ����Ͽ� �̵��ϴ� ���� �ǹ�
													// ��, Ŭ���̾�Ʈ�� ��õ� ��Ȯ�� ���̳� ������ �̵���Ų��. 
													// �� �ɼ��� ����� ���� URL�� ������ �����Ǿ�� �Ѵ�.
	}
}

////////// ��������Ʈ ���ε� �Լ� ���� ���� ------------------------------------------------------------------------------


////////// ����� ������ �Լ� ���� ���� ----------------------------------------------------------------------------------

// ���� ���� �Լ�
void UKJH_GameInstance::Host()
{
	// ����, ���� �������̽��� ��ȿ�ϴٸ�,
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);  // ���� ���� ���� ���
		if (ExistingSession) // ������ �̹� �����Ѵٸ�
		{
			UE_LOG(LogTemp, Warning, TEXT("Existing session found. Destroying the session..."));
			SessionInterface->DestroySession(SESSION_NAME); // ������ ���� ������ �ı�
															// ����Ǹ� 'DestroySession'�� ��������Ʈ�� ������ �����Ѵ�. ��, �ٷ� ��������Ʈ�� ȣ��ȴ�.
		}

		else // ������ ���� ���
		{
			UE_LOG(LogTemp, Warning, TEXT("No existing session found. Creating a new session..."));
			CreateSession(); // ���ο� ���� ����
		}
	}
}

// ���� ���� �Լ�
void UKJH_GameInstance::Join(uint32 Index)
{
	// SessionInterface �Ǵ� SessionSearch�� ��ȿ���� �ʴٸ�, return
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface or SessionSearch is not valid."));
		return;
	}

	if (SessionInterface->GetNamedSession(SESSION_NAME)) // �̹� ���ǿ� ���ӵǾ� �ִ��� Ȯ��
	{
		UE_LOG(LogTemp, Warning, TEXT("Already connected to a session. Cannot join again."));
		return; // �̹� ���ӵ� ������ �ִٸ� �ٽ� �������� �ʵ��� ����
	}

	if (ServerWidget)
	{
		ServerWidget->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

// ���� ���� �Լ�
void UKJH_GameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings; // CreateSession�� ���� ���Ƿ� ���Ǽ����� ������ش�.
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") // OnlineSubsystem �� NULL �� ���õǸ� (NULL : ���� ���� ����)
		{
			SessionSettings.bIsLANMatch = true; // true �� : ���� ��Ʈ��ũ�� �ִ� ����� ã�� (���� ���� ����) 
		}

		else
		{
			SessionSettings.bIsLANMatch = false; // false �� : �ٸ� ��Ʈ��ũ�� ���� �����ϵ��� ��. (Steam, XBox �� �����÷��� ���� ����)
		}

		SessionSettings.NumPublicConnections = 5; // �÷��̾� ��
		SessionSettings.bShouldAdvertise = true; // �¶��ο��� ������ �� �� �ֵ�����. '�����Ѵ�'
		SessionSettings.bUsesPresence = true;

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings); // ������ �����Ѵ�. 
																		   // ����Ǹ� 'CreateSession'�� ��������Ʈ�� ������ �����Ѵ�. ��, �ٷ� ��������Ʈ�� ȣ��ȴ�.
																		   // ����(�÷��̾��ȣ, TEXT("�����̸�"), ���Ǽ���)

	// ServerUIFactory�� ���� ServerUI ���� ����
		ServerWidget = CreateWidget<UKJH_ServerWidget>(this, ServerWidgetFactory);
		ServerWidget->SetMyInterface(this);
		ServerWidget->Setup();
		ServerWidget->ShowCharacterSelect();
	}
}

// UI �Լ� (�׽�Ʈ��)
void UKJH_GameInstance::LoadStartMenu()
{

	// ServerUIFactory�� ���� ServerUI ���� ����
	ServerWidget = CreateWidget<UKJH_ServerWidget>(this, ServerWidgetFactory);

	ServerWidget -> SetMyInterface(this);
	ServerWidget -> Setup();


}


void UKJH_GameInstance::LoadInGameMenu()
{

	// ServerUIFactory�� ���� ServerUI ���� ����
	InGameWidget = CreateWidget<UKJH_WidgetSystem>(this, InGameWidgetFactory);

	InGameWidget->SetMyInterface(this);
	InGameWidget->Setup();

}

void UKJH_GameInstance::LoadServerMenuMap()
{
	// �÷��̾��� ù��° ��Ʈ�ѷ��� �����´�.
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController) // ��Ʈ�ѷ��� ������,
	{
		// ServerUI�� �ִ� ������ �̵���Ų��.
		PlayerController->ClientTravel("/Game/MAPS/KJH/ServerWidgetMap.ServerWidgetMap", ETravelType::TRAVEL_Absolute);
	}
}

////////// ĳ���� ���� ���� �Լ� ----------------------------------------------------------------------------------------------------------------
void UKJH_GameInstance::OnCharacterSelected(bool bIsSelectedPersonFromUI)
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!PlayerController) return;

	// �÷��̾� ���� ��������
	PlayerState = PlayerController->GetPlayerState<AKJH_PlayerState>();
	if (!PlayerState) return;

	// ���õ� ĳ���� ������ ���� PlayerState ���� ����
	PlayerState->bIsPersonCharacterSelected = bIsSelectedPersonFromUI;

	// ������ ���� ������� RestartPlayer ȣ��
	GameMode = Cast<AKJH_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->RestartPlayer(PlayerController);
	}

	// UI ���� ���¸� ������Ʈ
	bIsPersonSelected = bIsSelectedPersonFromUI; // UI�κ��� ����� ������ ���� true �� ����� �����ߴٴ� ���� true�� �����ϰ�, false��� �����ߴٴ� ���� false�� ����
	bIsDroneSelected = !bIsSelectedPersonFromUI; // Drone�� �ݴ�� UI�κ��� false�� �����ߴٰ� ���� ����.
}