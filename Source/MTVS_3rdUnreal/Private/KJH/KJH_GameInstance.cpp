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

// 세션 생성에 사용할 수 있는 세션 이름을 전역 상수로 정의
const static FName SESSION_NAME = TEXT("EXIT Session Game");

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // 에디터 실행할 때 실행하는 생성자.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // 플레이를 눌렀을 때만 실행하는 생성자. 초기화만 시켜준다.
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(); // OnlineSubsystem 가져오기
	if (Subsystem) // 만약, Subsystem이 유효하다면,
	{

		SessionInterface = Subsystem->GetSessionInterface(); // 세션 인터페이스 가져오기
		
		// 만약, 세션 인터페이스가 유효하다면,
		if (SessionInterface.IsValid())
		{									// 1번은 예를 들어, CreateSession의 정보값을 받아서 OnCreateSessionComplete 함수 실행한다는 뜻. 
											// (Enhanced Input 처럼 바인딩한다고 생각하면 쉽다. 나머지도 비슷하게 바인딩.
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnJoinSessionComplete);
		}
	}
}

////////// 델리게이트 바인딩 함수 구간 시작 ------------------------------------------------------------------------------
void UKJH_GameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	// 세션이 성공적으로 생성된 후에는 UI를 제거하는 Teardown 함수를 실행한다.
	if (ServerWidget)
	{
		ServerWidget->Teardown();
	}

	// 내가 설정한 맵으로 listen 서버를 열고 이동한다.
	GetWorld()->ServerTravel(TEXT("/Game/MAPS/KJH/KJH_TestMap?listen"));

	//// 내가 설정한 맵으로 listen 서버를 열고 이동한다.
	//GetWorld()->ServerTravel(TEXT("/Game/ProtoMap/Proto_jsy?listen"));
}


// 세션 파괴 완료 시 호출되는 함수
void UKJH_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) // 파괴에 성공하면?
	{
		CreateSession(); // 세션을 만들어버린다. (기존에 세션이 있으면 그것을 부수고 새로운 세션을 만든다는 것이다.)
	}
}


void UKJH_GameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true; // LAN 사용 여부, true 면 LAN 세션을 찾고 false 면 인터넷 세션을 찾음.
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		// ToSharedRef -> TSharedPtr 을 항상 유효하게 바꿔주는 메서드. TSharedptr 은 Null일 수도 있는데, 
		// FindSession이란 메서드는 Null이면 위험하니까 애초에 유효한 녀석만 넣게 요청한다. 
		// 그래서 우리가 항상 유효하게 ToSharedRef로 변환해줘야한다.
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

	TArray<FString> ServerNames;
	//ServerNames.Add("Test Servr1"); // 테스트 텍스트를 생성
	//ServerNames.Add("Test Servr2");
	//ServerNames.Add("Test Servr3");
	if (SessionSearch->SearchResults.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete: No sessions found."));
	}

	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		FString SessionName = SearchResult.GetSessionIdStr();
		if (SessionName.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session with empty name."));
		}
		else
		{
			ServerNames.Add(SessionName);
			UE_LOG(LogTemp, Warning, TEXT("Found Session Name: %s"), *SessionName);
		}
	}

	// ServerWidget이 유효하고 SetServerList 호출이 안전한 경우에만 실행
	if (ServerWidget)
	{
		ServerWidget->SetServerList(ServerNames);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ServerWidget is not valid during SetServerList call."));
	}
}

// 세션에 들어올 경우 호출되는 함수
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
													// 해당 옵션은 절대 경로를 사용하여 이동하는 것을 의미
													// 즉, 클라이언트를 명시된 정확한 맵이나 서버로 이동시킨다. 
													// 이 옵션을 사용할 때는 URL이 완전히 지정되어야 한다.
	}
}

////////// 델리게이트 바인딩 함수 구간 종료 ------------------------------------------------------------------------------


////////// 사용자 정의형 함수 구간 시작 ----------------------------------------------------------------------------------

// 서버 열기 함수
void UKJH_GameInstance::Host()
{
	// 만약, 세션 인터페이스가 유효하다면,
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);  // 현재 세션 정보 얻기
		if (ExistingSession) // 세션이 이미 존재한다면
		{
			UE_LOG(LogTemp, Warning, TEXT("Existing session found. Destroying the session..."));
			SessionInterface->DestroySession(SESSION_NAME); // 기존에 명명된 세션을 파괴
															// 실행되면 'DestroySession'이 델리게이트에 정보를 제공한다. 즉, 바로 델리게이트가 호출된다.
		}

		else // 세션이 없을 경우
		{
			UE_LOG(LogTemp, Warning, TEXT("No existing session found. Creating a new session..."));
			CreateSession(); // 새로운 세션 생성
		}
	}
}

// 서버 접속 함수
void UKJH_GameInstance::Join(uint32 Index)
{
	// SessionInterface 또는 SessionSearch가 유효하지 않다면, return
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface or SessionSearch is not valid."));
		return;
	}

	if (SessionInterface->GetNamedSession(SESSION_NAME)) // 이미 세션에 접속되어 있는지 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("Already connected to a session. Cannot join again."));
		return; // 이미 접속된 세션이 있다면 다시 접속하지 않도록 방지
	}

	if (ServerWidget)
	{
		ServerWidget->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

// 세션 생성 함수
void UKJH_GameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings; // CreateSession을 위해 임의로 세션세팅을 만들어준다.
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") // OnlineSubsystem 이 NULL 로 세팅되면 (NULL : 로컬 연결 설정)
		{
			SessionSettings.bIsLANMatch = true; // true 시 : 같은 네트워크에 있는 사람을 찾음 (로컬 연결 설정) 
		}

		else
		{
			SessionSettings.bIsLANMatch = false; // false 시 : 다른 네트워크와 연결 가능하도록 함. (Steam, XBox 등 공식플랫폼 연결 설정)
		}

		SessionSettings.NumPublicConnections = 5; // 플레이어 수
		SessionSettings.bShouldAdvertise = true; // 온라인에서 세션을 볼 수 있도록함. '광고한다'
		SessionSettings.bUseLobbiesIfAvailable = true; // 로비기능을 활성화한다. (Host 하려면 필요)
		SessionSettings.bUsesPresence = true;

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings); // 세션을 생성한다. 
																		   // 실행되면 'CreateSession'이 델리게이트에 정보를 제공한다. 즉, 바로 델리게이트가 호출된다.
																		   // 인자(플레이어번호, TEXT("세션이름"), 세션세팅)
	}
}

////////// UI 관련 함수 ----------------------------------------------------------------------------------------------------------------
void UKJH_GameInstance::LoadServerWidget()
{
	// ServerUIFactory를 통해 ServerUI 위젯 생성
	ServerWidget = CreateWidget<UKJH_ServerWidget>(this, ServerWidgetFactory);
	ServerWidget -> SetMyInterface(this);
	ServerWidget -> Setup();
}

void UKJH_GameInstance::LoadInGameWidget()
{
	// ServerUIFactory를 통해 ServerUI 위젯 생성
	InGameWidget = CreateWidget<UKJH_WidgetSystem>(this, InGameWidgetFactory);
	InGameWidget->SetMyInterface(this);
	InGameWidget->Setup();
}



void UKJH_GameInstance::LoadServerWidgetMap()
{
	// 플레이어의 첫번째 컨트롤러를 가져온다.
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController) // 컨트롤러가 있으면,
	{
		// ServerUI가 있는 맵으로 이동시킨다.
		PlayerController->ClientTravel("/Game/MAPS/KJH/ServerWidgetMap.ServerWidgetMap", ETravelType::TRAVEL_Absolute);
	}
}

////////// 캐릭터 선택 관련 함수 ----------------------------------------------------------------------------------------------------------------
// 캐릭터 선택 관련 함수: PlayerController를 인자로 받아 각 플레이어의 선택을 독립적으로 처리
void UKJH_GameInstance::OnCharacterSelected(APlayerController* PlayerController, bool bIsSelectedPersonFromUI)
{
	if (!PlayerController) return;

	// PlayerState 가져오기
	AKJH_PlayerState* PlayerState = PlayerController->GetPlayerState<AKJH_PlayerState>();
	if (!PlayerState) return;

	// 로컬 컨트롤러만 처리하도록 설정
	if (PlayerController->IsLocalController())
	{
		// 클라이언트에서 선택 상태를 서버에 알림
		if (!PlayerController->HasAuthority())
		{
			ServerNotifyCharacterSelected(PlayerController, bIsSelectedPersonFromUI);
			return;
		}
	}

	// 서버에서 PlayerState를 업데이트하고 캐릭터 스폰을 처리
	if (PlayerController->HasAuthority())
	{
		PlayerState->bIsPersonCharacterSelected = bIsSelectedPersonFromUI;
		PlayerState->ForceNetUpdate();

		// 캐릭터 스폰 처리
		AKJH_PlayerController* KJHController = Cast<AKJH_PlayerController>(PlayerController);
		if (KJHController)
		{
			KJHController->bIsPersonCharacterSelected = bIsSelectedPersonFromUI;
			KJHController->SpawnCharacterBasedOnSelection();
		}
	}

	// UI 선택 상태를 업데이트
	bIsPersonSelected = bIsSelectedPersonFromUI;
	bIsDroneSelected = !bIsSelectedPersonFromUI;
}

bool UKJH_GameInstance::ServerNotifyCharacterSelected_Validate(APlayerController* PlayerController, bool bIsSelectedPerson)
{
	// 서버 RPC 요청의 유효성 검사
	return true;
}

void UKJH_GameInstance::ServerNotifyCharacterSelected_Implementation(APlayerController* PlayerController, bool bIsSelectedPerson)
{
	// 서버에서 캐릭터 선택을 처리
	OnCharacterSelected(PlayerController, bIsSelectedPerson);
}
