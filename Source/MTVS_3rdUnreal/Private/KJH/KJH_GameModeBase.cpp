// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_GameModeBase.h"
#include "KJH/KJH_PlayerState.h"
#include "JSH/JSH_Player.h"
#include "KHS/KHS_DronePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "KJH/KJH_ServerWidget.h"
#include "KJH/KJH_WidgetSystem.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_GameInstance.h"
#include "KJH/KJH_CharacterSelectWidget.h"
#include "KJH/KJH_PlayerController.h"

// GameMode 생성자에서 초기 설정
AKJH_GameModeBase::AKJH_GameModeBase()
{
    // 플레이어가 게임 시작 시 자동으로 시작되지 않고 관전자로 시작되게 설정
    bStartPlayersAsSpectators = true;
}

void AKJH_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // 기본적으로 RestartPlayer를 호출하지 않고, 특정 이벤트나 조건에서만 호출되도록 조정
    // 예를 들어, 플레이어가 캐릭터를 선택할 때 호출되도록 함

    // 캐릭터 선택 후에만 RestartPlayer가 호출되도록 설정
    // 실제 캐릭터 선택 로직에서 GameMode의 RestartPlayer를 호출하도록 설정함
    // 이건 GameInstance의 OnCharacterSelected 에서 처리함.
}

void AKJH_GameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    FString Value;
    UGameplayStatics::ParseOption(TEXT("PlayerSelected"), Value);

    UE_LOG(LogTemp,Warning, TEXT("PreLogin"));

    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}


void AKJH_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Warning, TEXT("PostLogin"));

    // 필요한 경우 추가적인 초기화 작업을 여기에 삽입
    // 예: 플레이어 상태 초기화, 게임 상태 업데이트 등
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);
}

////////// RPC 함수 구간 ------------------------------------------------------------------------------------------------


////////// 사용자 정의형 함수 구간 ---------------------------------------------------------------------------------------------

