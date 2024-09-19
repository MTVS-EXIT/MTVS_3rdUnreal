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
#include "Engine/TimerHandle.h"
#include "KJH/KJH_CharacterSelectWidget.h"
#include "KJH/KJH_PlayerController.h"
#include "KJH/KJH_LoginWidget.h"
#include "KJH/KJH_LoadingWidget.h"

// ���� ������ ����� �� �ִ� ���� �̸��� ���� ����� ����
const static FName SESSION_NAME = TEXT("EXIT Session Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // ������ ������ �� �����ϴ� ������.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // �÷��̸� ������ ���� �����ϴ� ������. �ʱ�ȭ�� �����ش�.
{
	//// ���� �ʱ�ȭ �� ȣ��Ǵ� ��������Ʈ ���ε�
	//FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UKJH_GameInstance::OnPostWorldInitialization);

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
	// ���� ���� ���� ��,
	if (!Success)
	{
		if (LoadingWidget)
			LoadingWidget->Teardown();
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	// ������ ���������� ���� ��,
	if (ServerWidget) // ServerWidget ����
		ServerWidget->Teardown();

	if (LoadingWidget) // LoadingWidget ����
		LoadingWidget->Setup();

	// �� ��ȯ ��, �񵿱� �ε��� ���� -> ������ OnMapPreloadComplete�� ȣ���Ͽ� ServerTravel ����
	StreamableManager.RequestAsyncLoad(FSoftObjectPath(TEXT("/Game/MAPS/TA_JSY/0_AlphaMap/AlphaMap")),
		FStreamableDelegate::CreateUObject(this, &UKJH_GameInstance::OnMapPreloadComplete));
}

void UKJH_GameInstance::OnMapPreloadComplete()
{
	if (LoadingWidget)
		LoadingWidget->Teardown();

	// ���� ������ ������ listen ������ ���� �̵��Ѵ�.
	//GetWorld()->ServerTravel(TEXT("/Game/MAPS/KJH/KJH_TestMap?listen"));

	// ������ ������ listen ������ ���� �̵��Ѵ�.
	//GetWorld()->ServerTravel(TEXT("/Game/Blueprints/Player/JSH_TMap?listen"));

	GetWorld()->ServerTravel(TEXT("/Game/MAPS/TA_JSY/0_AlphaMap/AlphaMap?listen"));
}

// ���� �ı� �Ϸ� �� ȣ��Ǵ� �Լ�
void UKJH_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) // �ı��� �����ϸ�?
	{
		CreateSession(); // ������ ����������. (������ ������ ������ �װ��� �μ��� ���ο� ������ ����ٴ� ���̴�.)
	}

	if (nullptr != GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UKJH_GameInstance::OnNetworkFailure);
	}
}


void UKJH_GameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	LoadServerWidgetMap();
}

void UKJH_GameInstance::RefreshServerList()
{

	// ���� ���� �˻� ��� �ʱ�ȭ
	if (SessionSearch.IsValid())
	{
		SessionSearch.Reset();
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true; // LAN ��� ����, true �� LAN ������ ã�� false �� ���ͳ� ������ ã��.
		SessionSearch->bIsLanQuery = false; // LAN ���� �˻� ���� ����
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		// ToSharedRef -> TSharedPtr �� �׻� ��ȿ�ϰ� �ٲ��ִ� �޼���. TSharedptr �� Null�� ���� �ִµ�, 
		// FindSession�̶� �޼���� Null�̸� �����ϴϱ� ���ʿ� ��ȿ�� �༮�� �ְ� ��û�Ѵ�. 
		// �׷��� �츮�� �׻� ��ȿ�ϰ� ToSharedRef�� ��ȯ������Ѵ�.
	}
}

void UKJH_GameInstance::OnFindSessionComplete(bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Error, TEXT("OnFindSessionComplete failed: Session search failed."));
		return;
	}

	if (!SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OnFindSessionComplete failed: SessionSearch is not valid."));
		return;
	}

	if (!ServerWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("OnFindSessionComplete failed: ServerWidget is not valid."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));

	TArray<FServerData> ServerNames;

	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		FServerData Data;
		Data.Name = SearchResult.GetSessionIdStr();
		Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections; // ���尡���� �ִ� �÷��̾� ��
		Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections; 
							 // �ִ� �÷��̾� �� - ����ִ� ������ �� = ���� ���� �÷��̾� ��
		Data.HostUserName = SearchResult.Session.OwningUserName;
		FString ServerName;
		if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
		{
			Data.Name = SearchResult.GetSessionIdStr();
		}
		else
		{
			Data.Name = "Could not Find Name";
		}

		ServerNames.Add(Data);
	}

	// ServerWidget�� ��ȿ�ϰ� SetServerList ȣ���� ������ ��쿡�� ����
	if (ServerWidget)
	{
		ServerWidget->SetServerList(ServerNames);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ServerWidget is not valid during SetServerList call."));
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
void UKJH_GameInstance::Host(FString ServerName)
{
	 DesiredServerName = ServerName;

	// ����, ���� �������̽��� ��ȿ�ϴٸ�,
	if (SessionInterface.IsValid())
	{
		// LoadingWidget �ʱ�ȭ
		if (LoadingWidgetFactory)
			LoadingWidget = CreateWidget<UKJH_LoadingWidget>(this, LoadingWidgetFactory);

		if (LoadingWidget)
		LoadingWidget -> Setup();

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
		SessionSettings.bUseLobbiesIfAvailable = true; // �κ����� Ȱ��ȭ�Ѵ�. (Host �Ϸ��� �ʿ�)
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings); // ������ �����Ѵ�. 
																		   // ����Ǹ� 'CreateSession'�� ��������Ʈ�� ������ �����Ѵ�. ��, �ٷ� ��������Ʈ�� ȣ��ȴ�.
																		   // ����(�÷��̾��ȣ, TEXT("�����̸�"), ���Ǽ���)
	}
}


// UI ���� ���� �Լ� ----------------------------------------------------------------------------------------------------------------
// �α��� UI ���� �Լ�
void UKJH_GameInstance::CreateLoginWidget()
{
	// LoginWidgetFactory�� ���� LogInUI ���� ����
	LoginWidget = CreateWidget<UKJH_LoginWidget>(this, LoginWidgetFactory);
	LoginWidget->SetMyInterface(this);
	LoginWidget->Setup();
}

// ���� ���θ޴� UI ���� �Լ�
void UKJH_GameInstance::CreateServerWidget()
{
	// ServerUIFactory�� ���� ServerUI ���� ����
	ServerWidget = CreateWidget<UKJH_ServerWidget>(this, ServerWidgetFactory);
	ServerWidget -> SetMyInterface(this);
	ServerWidget -> Setup();
}

// �ΰ��� UI ���� �Լ�
void UKJH_GameInstance::CreateInGameWidget()
{
	// InGameWidgetFactory�� ���� InGameUI ���� ����
	InGameWidget = CreateWidget<UKJH_InGameWidget>(this, InGameWidgetFactory);
	InGameWidget->SetMyInterface(this);
	InGameWidget->Setup();
}

void UKJH_GameInstance::LoadServerWidgetMap()
{
	// AKJH_PlayerController�� �����´�,
	AKJH_PlayerController* KJHPlayerController = Cast<AKJH_PlayerController>(GetFirstLocalPlayerController());
	//// �÷��̾��� ù��° ��Ʈ�ѷ��� �����´�.
	//APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (KJHPlayerController && KJHPlayerController->IsLocalController()) // ��Ʈ�ѷ��� ������,
	{
		// ServerUI�� �ִ� ������ �̵���Ų��.
		KJHPlayerController->ClientTravel("/Game/MAPS/KJH/ServerWidgetMap", ETravelType::TRAVEL_Absolute);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in LoadServerWidgetMap."));
	}
}

////////// ĳ���� ���� ���� �Լ� ----------------------------------------------------------------------------------------------------------------
// ĳ���� ���� ���� �Լ�: PlayerController�� ���ڷ� �޾� �� �÷��̾��� ������ ���������� ó��
void UKJH_GameInstance::OnCharacterSelected(APlayerController* PlayerController, bool bIsSelectedPersonFromUI)
{
	if (!PlayerController) return;

	// PlayerState ��������
	AKJH_PlayerState* PlayerState = PlayerController->GetPlayerState<AKJH_PlayerState>();
	if (!PlayerState) return;

	// ���� ��Ʈ�ѷ��� ó���ϵ��� ����
	if (PlayerController->IsLocalController())
	{
		// Ŭ���̾�Ʈ���� ���� ���¸� ������ �˸�
		if (!PlayerController->HasAuthority())
		{
			ServerNotifyCharacterSelected(PlayerController, bIsSelectedPersonFromUI);
			return;
		}
	}

	// �������� PlayerState�� ������Ʈ�ϰ� ĳ���� ������ ó��
	if (PlayerController->HasAuthority())
	{
		PlayerState->bIsPersonCharacterSelected = bIsSelectedPersonFromUI;
		PlayerState->ForceNetUpdate();

		// ĳ���� ���� ó��
		AKJH_PlayerController* KJHController = Cast<AKJH_PlayerController>(PlayerController);
		if (KJHController)
		{
			KJHController->bIsPersonCharacterSelected = bIsSelectedPersonFromUI;
			KJHController->SpawnCharacterBasedOnSelection();
		}
	}

	// UI ���� ���¸� ������Ʈ
	bIsPersonSelected = bIsSelectedPersonFromUI;
	bIsDroneSelected = !bIsSelectedPersonFromUI;
}

bool UKJH_GameInstance::ServerNotifyCharacterSelected_Validate(APlayerController* PlayerController, bool bIsSelectedPerson)
{
	// ���� RPC ��û�� ��ȿ�� �˻�
	return true;
}

void UKJH_GameInstance::ServerNotifyCharacterSelected_Implementation(APlayerController* PlayerController, bool bIsSelectedPerson)
{
	// �������� ĳ���� ������ ó��
	OnCharacterSelected(PlayerController, bIsSelectedPerson);
}


////////// Temp ================================================================================================================================
//void UKJH_GameInstance::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
//{
//	if (World)
//	{
//		// �ε�� ������ �� �̸� Ȯ��
//		FString MapName = World->GetMapName();
//
//		// �� �̸��� "ServerWidgetMap"�� ���ԵǾ� �ִٸ� ���� ����
//		if (MapName.Contains("ServerWidgetMap"))
//		{
//			UE_LOG(LogTemp, Log, TEXT("ServerWidgetMap detected, creating server widget..."));
//			// 0.1�� Ÿ�̸� ����. ���� �����̴� Tick���� ó���ǹǷ� �����ϰ� ó����.
//
//			// ���� ������ Ÿ�̸� �ڵ�
//			FTimerHandle WidgetCreationTimerHandle;
//			GetWorld()->GetTimerManager().SetTimer(WidgetCreationTimerHandle, this, &UKJH_GameInstance::CreateServerWidget, 0.01f, false);
//		}
//	}
//}
