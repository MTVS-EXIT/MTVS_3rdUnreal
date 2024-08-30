// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_Interface.h"
#include "KJH/KJH_UserWidget.h"

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
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnCreateSessionComplete);
																// CreateSession의 정보값을 받아서 OnCreateSessionComplete 함수 실행

			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnFindSessionComplete);

			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			if (SessionSearch.IsValid())
			{

			SessionSearch->bIsLanQuery = true;
			
			SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
			// ToSharedRef -> TSharedPtr 을 항상 유효하게 바꿔주는 메서드. TSharedptr 은 Null일 수도 있는데, 
			// FindSession이란 메서드는 Null이면 위험하니까 애초에 유효한 녀석만 넣게 요청한다. 그래서 우리가 항상 유효하게 ToSharedRef로 변환해줘야한다.
			}

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

	// 내가 설정한 맵으로 listen 서버를 열어준다.
	GetWorld()->ServerTravel("/Game/MAPS/KJH/KJH_TestMap?listen");

	// 세션이 성공적으로 생성된 후에는 UI를 제거하는 Teardown 함수를 실행한다.
	if (ServerUI)
	{
		ServerUI->Teardown();
	}
}


// 세션 파괴 완료 시 호출되는 함수
void UKJH_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) // 파괴에 성공하면?
	{
		CreateSession(); // 세션을 만들어버린다. (기존에 세션이 있으면 그것을 부수고 새로운 세션을 만든다는 것이다.)
	}
}


void UKJH_GameInstance::OnFindSessionComplete( bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));

	if (Success && SessionSearch.IsValid())
	{
		for (auto& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session Names : %s"), *SearchResult.GetSessionIdStr());
		}
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
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME); // 현재 세션 정보 얻기
		if (ExistingSession) // 만약, 현재 세션이 존재하면
		{
			SessionInterface->DestroySession(SESSION_NAME); // 기존에 명명된 세션을 파괴
															// 실행되면 'DestroySession'이 델리게이트에 정보를 제공한다. 즉, 바로 델리게이트가 호출된다.
		}

		else // 만약, 현재 세션이 없다면,
		{
			CreateSession(); // 세션을 만든다.
		}
	}
}

// 서버 접속 함수
void UKJH_GameInstance::Join(const FString& Address)
{
	GEngine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
													// 해당 옵션은 절대 경로를 사용하여 이동하는 것을 의미
													// 즉, 클라이언트를 명시된 정확한 맵이나 서버로 이동시킨다. 
													// 이 옵션을 사용할 때는 URL이 완전히 지정되어야 한다.
	}
}

// 세션 생성 함수
void UKJH_GameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings; // CreateSession을 위해 임의로 세션세팅을 만들어준다.
		SessionSettings.bIsLANMatch = true ; // 로컬에서 찾을 것인가?
		SessionSettings.NumPublicConnections = 5; // 플레이어 수
		SessionSettings.bShouldAdvertise = true; // 온라인에서 세션을 볼 수 있도록함. '광고함'

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings); // 세션을 생성한다. 
																		   // 실행되면 'CreateSession'이 델리게이트에 정보를 제공한다. 즉, 바로 델리게이트가 호출된다.
																		   // 인자(플레이어번호, TEXT("세션이름"), 세션세팅)
	}
}

// UI 함수 (테스트용)
void UKJH_GameInstance::LoadMenu()
{

	// ServerUIFactory를 통해 ServerUI 위젯 생성
	ServerUI = CreateWidget<UKJH_MainServerUI>(this, ServerUIFactory);

	ServerUI -> Setup();

	ServerUI -> SetMyInterface(this);

}