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

    // PlayerController에서 캐릭터 선택 UI 생성
    AKJH_PlayerController* KJHController = Cast<AKJH_PlayerController>(NewPlayer);
    if (KJHController)
    {
        KJHController->ShowCharacterSelectWidget();
    }
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
 
    // 더 이상 캐릭터 스폰 로직을 포함하지 않고 기본 기능만 유지
    Super::RestartPlayer(NewPlayer);
}

////////// RPC 함수 구간 ------------------------------------------------------------------------------------------------
void AKJH_GameModeBase::Multicast_ShowCharacterSelectWidget_Implementation(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid for showing CharacterSelectWidget."));
        return;
    }

    // 이 함수는 클라이언트에서만 실행됨, 따라서 서버는 여기서 UI 생성 안함
    if (PlayerController->IsLocalController()) // 클라이언트에서만 실행
    {
        // PlayerController에서 ShowCharacterSelectWidget 호출
        AKJH_PlayerController* KJHController = Cast<AKJH_PlayerController>(PlayerController);
        if (KJHController)
        {
            KJHController->ShowCharacterSelectWidget();
        }
    }
}

////////// 사용자 정의형 함수 구간 ---------------------------------------------------------------------------------------------

// 캐릭터 선택 UI 생성 함수
void AKJH_GameModeBase::ShowCharacterSelectWidget(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid for showing CharacterSelectWidget."));
        return;
    }

    UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
    if (GameInstance)
    {
        // 개별적으로 각 플레이어마다 위젯 생성 및 설정 (CharacterSelectWidget 멤버 변수를 사용하지 않음)
        UKJH_CharacterSelectWidget* IndividualCharacterSelectWidget = CreateWidget<UKJH_CharacterSelectWidget>(PlayerController, CharacterSelectWidgetFactory);

        if (IndividualCharacterSelectWidget)
        {
            IndividualCharacterSelectWidget->AddToViewport();
            IndividualCharacterSelectWidget->Setup();
            UE_LOG(LogTemp, Warning, TEXT("CharacterSelectWidget Created and Setup for Player."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CharacterSelectWidget! Check if the Factory is valid."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to UKJH_GameInstance."));
    }
}
