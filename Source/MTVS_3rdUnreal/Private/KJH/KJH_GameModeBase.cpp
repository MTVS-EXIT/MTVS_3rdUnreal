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

    // 새로 접속한 플레이어는 관전 모드로 설정
    if (NewPlayer)
    {
        APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
        if (PlayerController)
        {
            // 관전 모드로 설정은 캐릭터 선택 전까지만 필요
            PlayerController->ChangeState(NAME_Spectating);
        }
    }

    // 서버에서 위젯 생성
    if (HasAuthority()) // 서버에서만 실행
    {
        ShowCharacterSelectWidget(NewPlayer);
    }
    // 클라이언트에게 Character Select UI를 표시하라는 요청
    Multicast_ShowCharacterSelectWidget(NewPlayer);
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);

    if (!NewPlayer) return;

    // 플레이어 상태를 가져옴
    AKJH_PlayerState* PlayerState = NewPlayer->GetPlayerState<AKJH_PlayerState>();

    // 플레이어 컨트롤러 가져오기
    APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
    if (!PlayerController) return;

    // 관전 모드에서 플레이 모드로 전환할 때만 작동하게 설정
    if (PlayerState)
    {
        // 각 플레이어의 PlayerState에 따라 캐릭터 클래스를 선택
        TSubclassOf<APawn> ChosenCharacterClass = PlayerState->bIsPersonCharacterSelected
            ? BP_JSH_PlayerClass // true 면 사람이 선택되었다고 생각하고 사람 BP클래스로 설정
            : BP_KHS_DronePlayerClass; // false면 드론이 선택되었다고 생각하고 드론 BP클래스로 설정

        // 디버그 메시지로 선택된 캐릭터 클래스 출력
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue,
            FString::Printf(TEXT("Chosen Character Class: %s"), *ChosenCharacterClass->GetName()));


        // 스폰 파라미터 설정
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = NewPlayer;
        SpawnParams.Instigator = NewPlayer->GetPawn();

        // 스폰 위치와 회전 값 설정
        FVector SpawnLocation = FVector(0.f, 0.f, 100.f); // 기본 스폰 위치 설정 (필요에 따라 수정)
        FRotator SpawnRotation = FRotator::ZeroRotator; // 기본 회전 값 설정

        // 스폰 전 디버그 메시지 추가
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Attempting to spawn character..."));

        // 새로운 캐릭터 스폰
        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewPawn)
        {
            // 플레이어가 새 캐릭터를 조종하도록 설정
            PlayerController->Possess(NewPawn);

            // 관전자 모드에서 플레이 모드로 전환
            PlayerController->ChangeState(NAME_Playing);

            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Character successfully spawned!"));
            UE_LOG(LogTemp,Warning, TEXT("Character successfully spawned!"));
        }

        else
        {
            // PlayerState가 없는 경우 기본 로직 실행
            Super::RestartPlayer(NewPlayer);
        }
    }
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
    if (!HasAuthority()) // 클라이언트에서만 실행
    {
        ShowCharacterSelectWidget(PlayerController);
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
