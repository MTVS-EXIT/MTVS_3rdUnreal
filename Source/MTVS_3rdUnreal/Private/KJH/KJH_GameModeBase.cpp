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

        // 1. 플레이어를 Spectator(관전자) 모드로 시작하게 설정
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (PlayerController)
        {
            // 플레이어를 Spectator 모드로 설정하여 자동으로 캐릭터를 Possess하지 않도록 함
            PlayerController->ChangeState(NAME_Spectating);
        }
    }

    // 2. 캐릭터 선택 후에만 RestartPlayer가 호출되도록 설정
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

    // GameInstance에서 ServerWidget을 가져옵니다.
    UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetGameInstance());
    if (GameInstance)
    {
        // ServerWidget이 이미 존재하는지 확인
        if (!ServerWidget && ServerWidgetFactory)
        {
            // ServerWidget 생성
            ServerWidget = CreateWidget<UKJH_ServerWidget>(GameInstance, ServerWidgetFactory);
            if (ServerWidget)
            {
                // 인터페이스 설정
                ServerWidget->SetMyInterface(GameInstance);

                // UI 세팅
                ServerWidget->Setup(); // 이 함수가 UI를 화면에 추가하고 입력 모드를 설정합니다.

                ServerWidget->AddToViewport(); // AddToViewport로 추가하여 제대로 표시되게 설정


                UE_LOG(LogTemp, Warning, TEXT("ServerWidget Created and Setup."));
            }
        }
    }

    // ServerWidget이 유효하면 ShowCharacterSelect() 호출
    if (ServerWidget)
    {
        ServerWidget->ShowCharacterSelect();
        UE_LOG(LogTemp, Warning, TEXT("Character Select UI shown."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ServerWidget is not valid! Cannot show character select UI."));
    }
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);

    // 플레이어 상태를 가져옴
    PlayerState = NewPlayer->GetPlayerState<AKJH_PlayerState>();

    if (PlayerState)
    {
        // 선택된 캐릭터에 따라 스폰할 캐릭터 클래스를 설정
        TSubclassOf<APawn> ChosenCharacterClass = PlayerState->bIsPersonCharacterSelected
            ? BP_JSH_PlayerClass // true 면 사람이 선택되었다고 생각하고 사람 BP클래스로 설정
            : BP_KHS_DronePlayerClass; // false면 드론이 선택되었다고 생각하고 드론 BP클래스로 설정

        if (!ChosenCharacterClass)
        {
            UE_LOG(LogTemp, Error, TEXT("ChosenCharacterClass is not valid."));
            return;
        }

                    // 디버그 메시지로 선택된 캐릭터 클래스 출력
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue,
            FString::Printf(TEXT("Chosen Character Class: %s"), *ChosenCharacterClass->GetName()));


        // 스폰 파라미터 설정
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = NewPlayer;
        SpawnParams.Instigator = NewPlayer->GetPawn();

        // 스폰 위치와 회전 값 설정
        FVector SpawnLocation = FVector(0.f, 0.f, 300.f); // 기본 스폰 위치 설정 (필요에 따라 수정)
        FRotator SpawnRotation = FRotator::ZeroRotator; // 기본 회전 값 설정

        // 스폰 전 디버그 메시지 추가
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Attempting to spawn character..."));

        // 새로운 캐릭터 스폰
        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewPawn)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Character successfully spawned!"));
            UE_LOG(LogTemp,Warning, TEXT("Character successfully spawned!"));
            NewPlayer->Possess(NewPawn); // 플레이어가 새 캐릭터를 조종하도록 설정
        }

            // 관전자 모드를 해제하고 플레이어 상태로 전환
            APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
            if (PlayerController)
            {
                PlayerController->ChangeState(NAME_Playing); // Spectating 상태에서 Playing 상태로 전환
                bStartPlayersAsSpectators = false;
            }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn character!"));
        }

    }
    else
    {
        // 플레이어 상태가 없으면 기본 게임 모드 로직 사용
        Super::RestartPlayer(NewPlayer);
        UE_LOG(LogTemp, Error, TEXT("PlayerState is not valid."));
    }
}
