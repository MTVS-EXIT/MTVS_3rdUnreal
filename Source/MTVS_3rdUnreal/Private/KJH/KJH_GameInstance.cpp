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
#include "Kismet/GameplayStatics.h"

// 세션 생성에 사용할 수 있는 세션 이름을 전역 상수로 정의
const static FName SESSION_NAME = TEXT("EXIT Session Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // 에디터 실행할 때 실행하는 생성자.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // 플레이를 눌렀을 때만 실행하는 생성자. 초기화만 시켜준다.
{
	Super::Init();

	// ---------------------------------------------------------------------------------------------------------- //

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

void UKJH_GameInstance::OnStart()
{
	// 처음에는 로비 사운드 재생
	PlayLobbySound();
}

////////// 델리게이트 바인딩 함수 구간 시작 ------------------------------------------------------------------------------
void UKJH_GameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	// 세션 생성 실패 시,
	if (!Success)
	{
		if (LoadingWidget)
			LoadingWidget->Teardown();
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	// 세션이 성공적으로 생성 시,
	if (ServerWidget) // ServerWidget 제거
		ServerWidget->Teardown();

	if (LoadingWidget) // LoadingWidget 생성
		LoadingWidget->Setup();

	// 맵 전환 전, 비동기 로딩을 시작 -> 끝나면 OnMapPreloadComplete을 호출하여 ServerTravel 시작
	StreamableManager.RequestAsyncLoad(FSoftObjectPath(TEXT("/Game/MAPS/TA_JSY/00_BetaMap/BetaMap")),
		FStreamableDelegate::CreateUObject(this, &UKJH_GameInstance::OnMapPreloadComplete));
}

void UKJH_GameInstance::OnMapPreloadComplete()
{
	if (LoadingWidget)
		LoadingWidget->Teardown();

	// 내가 설정한 맵으로 listen 서버를 열고 이동한다.
	//GetWorld()->ServerTravel(TEXT("/Game/MAPS/KJH/KJH_TestMap?listen"));

	// 수혁이 맵으로 listen 서버를 열고 이동한다.
	//GetWorld()->ServerTravel(TEXT("/Game/Blueprints/Player/JSH_TMap?listen"));
	
	StopCurrentSound(); // 기존 사운드 중지

	//PlayStageSound(); // 베타맵으로 이동하기 직전에 스테이지 사운드로 전환

	GetWorld()->ServerTravel(TEXT("/Game/MAPS/TA_JSY/00_BetaMap/BetaMap?listen"));

	// 맵 전환 후 약간의 지연을 두고 StageSound 재생
	FTimerHandle TimerHandle_StageSound;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_StageSound, this, &UKJH_GameInstance::PlayStageSound, 3.0f, false);

	// 맵 전환 후 약간의 지연을 두고 StageSound 재생
	FTimerHandle TimerHandle_BreathSound;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_BreathSound, this, &UKJH_GameInstance::PlayBreathSound, 4.0f, false);
}

void UKJH_GameInstance::SetUserId(const FString& NewUserId)
{
	UserId = NewUserId;
}

FString UKJH_GameInstance::GetUserId() const
{
	return UserId;
}

// 세션 파괴 완료 시 호출되는 함수
void UKJH_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) // 파괴에 성공하면?
	{
		CreateSession(); // 세션을 만들어버린다. (기존에 세션이 있으면 그것을 부수고 새로운 세션을 만든다는 것이다.)
	}

	if (nullptr != GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UKJH_GameInstance::OnNetworkFailure);
	}
}


void UKJH_GameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// 현재 사운드를 중지
	StopCurrentSound();

	// 서버 위젯 맵으로 이동
	LoadServerWidgetMap(false); // false 인자를 통해 현재 사운드를 유지하지 않으며 이동

	// 로비 사운드 재생
	PlayLobbySound();
}

void UKJH_GameInstance::RefreshServerList()
{

	// 기존 세션 검색 결과 초기화
	if (SessionSearch.IsValid())
	{
		SessionSearch.Reset();
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true; // LAN 사용 여부, true 면 LAN 세션을 찾고 false 면 인터넷 세션을 찾음.
		SessionSearch->bIsLanQuery = false; // LAN 세션 검색 여부 설정
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

	TArray<FServerData> ServerNames;

	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		FServerData Data;
		Data.Name = SearchResult.GetSessionIdStr();
		Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections; // 입장가능한 최대 플레이어 수
		Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections; 
							 // 최대 플레이어 수 - 비어있는 슬롯의 수 = 접속 중인 플레이어 수
		Data.HostUserName = SearchResult.Session.OwningUserName;
		FString ServerName;
		if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
		{
			Data.Name = ServerName;
			//Data.Name = SearchResult.GetSessionIdStr();
		}
		else
		{
			//Data.Name = "Could not Find Name";
			Data.Name = SearchResult.GetSessionIdStr(); // 서버 이름을 찾을 수 없는 경우, 고유 아이디를 가져옴
		}

		ServerNames.Add(Data);
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
		// 호스트 플레이어의 경우에도 ClientTravel을 호출
		if (PlayerController->GetLocalRole() == ROLE_Authority)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Relative);
		}
		else
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
													// 해당 옵션은 절대 경로를 사용하여 이동하는 것을 의미
													// 즉, 클라이언트를 명시된 정확한 맵이나 서버로 이동시킨다. 
													// 이 옵션을 사용할 때는 URL이 완전히 지정되어야 한다.
		}
	}
}


////////// 델리게이트 바인딩 함수 구간 종료 ------------------------------------------------------------------------------


////////// 사용자 정의형 함수 구간 시작 ----------------------------------------------------------------------------------

// 서버 열기 함수
void UKJH_GameInstance::Host(FString ServerName)
{
	 DesiredServerName = ServerName;

	// 만약, 세션 인터페이스가 유효하다면,
	if (SessionInterface.IsValid())
	{
		// LoadingWidget 초기화
		if (LoadingWidgetFactory)
			LoadingWidget = CreateWidget<UKJH_LoadingWidget>(this, LoadingWidgetFactory);

		if (LoadingWidget)
		LoadingWidget -> Setup();

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
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings); // 세션을 생성한다. 
																		   // 실행되면 'CreateSession'이 델리게이트에 정보를 제공한다. 즉, 바로 델리게이트가 호출된다.
																		   // 인자(플레이어번호, TEXT("세션이름"), 세션세팅)
	}
}


// UI 생성 관련 함수 ----------------------------------------------------------------------------------------------------------------
// 로그인 UI 생성 함수
void UKJH_GameInstance::CreateLoginWidget()
{
	// LoginWidgetFactory를 통해 LogInUI 위젯 생성
	LoginWidget = CreateWidget<UKJH_LoginWidget>(this, LoginWidgetFactory);
	LoginWidget->SetMyInterface(this);
	LoginWidget->Setup();
}

// 서버 메인메뉴 UI 생성 함수
void UKJH_GameInstance::CreateServerWidget()
{
	// ServerUIFactory를 통해 ServerUI 위젯 생성
	ServerWidget = CreateWidget<UKJH_ServerWidget>(this, ServerWidgetFactory);
	ServerWidget -> SetMyInterface(this);
	ServerWidget -> Setup();
}

// 인게임 UI 생성 함수
void UKJH_GameInstance::CreateInGameWidget()
{
	// InGameWidgetFactory를 통해 InGameUI 위젯 생성
	InGameWidget = CreateWidget<UKJH_InGameWidget>(this, InGameWidgetFactory);
	InGameWidget->SetMyInterface(this);
	InGameWidget->Setup();
}

void UKJH_GameInstance::LoadServerWidgetMap(bool bKeepCurrentSound) // true, false 인자를 통해 기존 사운드를 유지하지 않으면서 이동하겠다고 설정가능
{
	// AKJH_PlayerController를 가져온다,
	AKJH_PlayerController* KJHPlayerController = Cast<AKJH_PlayerController>(GetFirstLocalPlayerController());
	//// 플레이어의 첫번째 컨트롤러를 가져온다.
	//APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (KJHPlayerController && KJHPlayerController->IsLocalController()) // 컨트롤러가 있으면,
	{
		if (false == bKeepCurrentSound) // false 인자인 경우 기존 사운드를 유지하지 않으면서 이동
		{
			StopCurrentSound();
		}
		else
		{
			ContinueCurrentSound();
		}

		// ServerUI가 있는 맵으로 이동시킨다.
		KJHPlayerController->ClientTravel("/Game/MAPS/KJH/ServerWidgetMap", ETravelType::TRAVEL_Absolute);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in LoadServerWidgetMap."));
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

////////// 사운드 재생 관련 함수 --------------------------------------------------------------------------------------------------------------
// 로비 사운드 재생 함수
void UKJH_GameInstance::PlayLobbySound()
{
	if (LobbySound)
	{
		if (CurrentPlayingSound)
		{
			CurrentPlayingSound->Stop();
		}
		CurrentPlayingSound = UGameplayStatics::SpawnSound2D(this, LobbySound, 1.0f, 1.0f, 0.0f, nullptr, true, false);
		UE_LOG(LogTemp, Warning, TEXT("Started playing lobby sound"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LobbySound is not set"));
	}
}

// 시뮬레이션 스테이지 사운드 재생 함수
void UKJH_GameInstance::PlayStageSound()
{
	if (StageSound)
	{
		CurrentPlayingSound = UGameplayStatics::SpawnSound2D(this, StageSound, 1.0f, 1.0f, 0.0f);
		UE_LOG(LogTemp, Warning, TEXT("Started playing stage sound"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StageSound is not set"));
	}
}

// 현재 사운드 재생 중지 함수
void UKJH_GameInstance::StopCurrentSound()
{
	if (CurrentPlayingSound && CurrentPlayingSound->IsPlaying())
	{
		CurrentPlayingSound->Stop();
		UE_LOG(LogTemp, Warning, TEXT("Stopped current sound"));
	}
}

// 현재 사운드 유지 함수
void UKJH_GameInstance::ContinueCurrentSound()
{
	// 현재 재생 중인 사운드가 있다면 계속 재생
	if (CurrentPlayingSound && !CurrentPlayingSound->IsPlaying())
	{
		CurrentPlayingSound->Play();
	}
	else if (!CurrentPlayingSound)
	{
		// 현재 재생 중인 사운드가 없다면 로비 사운드 재생
		PlayLobbySound();
	}
}

void UKJH_GameInstance::PlayBreathSound()
{
	//if (BreathSound)
	//	UGameplayStatics::PlaySound2D(this, BreathSound, 10.0f, 1.0f, 0.0f);
}

void UKJH_GameInstance::StopBreathSound()
{
	if (BreathSoundComponent && BreathSoundComponent->IsPlaying())
	{
		BreathSoundComponent->Stop();
	}
}

////////// Temp ================================================================================================================================
//void UKJH_GameInstance::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
//{
//	if (World)
//	{
//		// 로드된 월드의 맵 이름 확인
//		FString MapName = World->GetMapName();
//
//		// 맵 이름에 "ServerWidgetMap"이 포함되어 있다면 위젯 생성
//		if (MapName.Contains("ServerWidgetMap"))
//		{
//			UE_LOG(LogTemp, Log, TEXT("ServerWidgetMap detected, creating server widget..."));
//			// 0.1초 타이머 설정. 실제 딜레이는 Tick마다 처리되므로 안전하게 처리됨.
//
//			// 위젯 생성용 타이머 핸들
//			FTimerHandle WidgetCreationTimerHandle;
//			GetWorld()->GetTimerManager().SetTimer(WidgetCreationTimerHandle, this, &UKJH_GameInstance::CreateServerWidget, 0.01f, false);
//		}
//	}
//}
