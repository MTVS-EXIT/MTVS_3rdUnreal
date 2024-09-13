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
}

////////// Login 관련 함수 선언 (PreLogin -> Login -> PostLogin 순서로 호출) ============================================================
// PreLogin
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

    ++NumberOfPlayers; // 플레이어가 입장하였으므로 증가

   // if (NumberOfPlayers >= 2)
   // {
   //     UWorld* World = GetWorld();
   //     if (false != World)
   //     {
   //         bUseSeamlessTravel = true;
   //         World -> ServerTravel(TEXT("/Game/ProtoMap/ProtoPT?listen"));
   //     }
   //}
}

void AKJH_GameModeBase::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    --NumberOfPlayers; // 플레이어가 퇴장하였으므로 감소
}

////////// 플레이어가 게임에 참여할 때 호출되어 캐릭터를 스폰하는 함수 ========================================================
void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);
}



