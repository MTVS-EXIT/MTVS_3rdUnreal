// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "KHS/KHS_DronePlayer.h"

void AKJH_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay에서 ShowCharacterSelectWidget을 호출하여 클라이언트에서만 UI를 생성하도록 설정
    if (IsLocalController()) // 로컬 클라이언트일 때만 실행
    {
        ShowCharacterSelectWidget();
    }
}

void AKJH_PlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    // 캐릭터가 Possess된 후에도 UI를 표시하도록 설정
    if (IsLocalController()) // 로컬 클라이언트일 때만 실행
    {
        ShowCharacterSelectWidget();
    }

    AKHS_DronePlayer* DronePlayer = Cast<AKHS_DronePlayer>(aPawn);
    if (DronePlayer)
    {
        if (IsLocalController())
        {
            Client_SetupDroneUI();
        }
    }

}

void AKJH_PlayerController::ShowCharacterSelectWidget()
{
    // 로컬 컨트롤러에서만 UI를 생성하도록 설정
    if (IsLocalPlayerController())
    {
        if (!CharacterSelectWidget && CharacterSelectWidgetFactory)
        {
            // 위젯 생성
            CharacterSelectWidget = CreateWidget<UKJH_CharacterSelectWidget>(this, CharacterSelectWidgetFactory);
            if (CharacterSelectWidget)
            {
                // 위젯 설정 및 뷰포트에 추가
                CharacterSelectWidget->Setup();
                CharacterSelectWidget->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("CharacterSelectWidget Created and Setup for Player."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create CharacterSelectWidget! Check if the Factory is valid."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to create CharacterSelectWidget on a non-local controller."));
    }
}

void AKJH_PlayerController::ClientShowCharacterSelectWidget_Implementation()
{
    ShowCharacterSelectWidget();
}




void AKJH_PlayerController::SpawnCharacterBasedOnSelection()
{
    if (!HasAuthority()) // 클라이언트에서 실행된 경우
    {
        ServerSpawnCharacterBasedOnSelection(bIsPersonCharacterSelected);
        return;
    }

    // 서버에서의 실행 로직
    TSubclassOf<APawn> ChosenCharacterClass = bIsPersonCharacterSelected
        ? BP_JSH_PlayerClass
        : BP_KHS_DronePlayerClass;

    // GameMode를 통해 스폰 위치 얻기
    AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
    FVector NewSpawnLocation;
    FRotator NewSpawnRotation;


    if (GameMode)
    {
        AActor* PlayerStartSpot = GameMode->FindPlayerStart(this);
        if (PlayerStartSpot)
        {
            NewSpawnLocation = PlayerStartSpot->GetActorLocation();
            NewSpawnRotation = PlayerStartSpot->GetActorRotation();
        }
        else
        {
            // 기본 위치 (필요한 경우 조정)
            NewSpawnLocation = FVector(0.f, 0.f, 100.f);
            NewSpawnRotation = FRotator::ZeroRotator;
        }
    }
    else
    {
        // 기본 위치 (필요한 경우 조정)
        NewSpawnLocation = FVector(0.f, 0.f, 100.f);
        NewSpawnRotation = FRotator::ZeroRotator;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetPawn();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenCharacterClass, NewSpawnLocation, NewSpawnRotation, SpawnParams);
    if (NewPawn)
    {
        // 네트워크 설정
        NewPawn->SetReplicates(true);
        NewPawn->SetReplicateMovement(true);

        Possess(NewPawn);
        UE_LOG(LogTemp, Warning, TEXT("Character successfully spawned for Player at location: %s"), *NewSpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn character for Player."));
    }
}

void AKJH_PlayerController::ServerSpawnCharacterBasedOnSelection_Implementation(bool bIsPersonSelected)
{
    bIsPersonCharacterSelected = bIsPersonSelected;
    SpawnCharacterBasedOnSelection();
}

bool AKJH_PlayerController::ServerSpawnCharacterBasedOnSelection_Validate(bool bIsPersonSelected)
{
	return true;
}

void AKJH_PlayerController::Client_SetupDroneUI_Implementation()
{
    AKHS_DronePlayer* DronePlayer = Cast<AKHS_DronePlayer>(GetPawn());
    if (DronePlayer && DronePlayer->DroneMainUIFactory)
    {
        DronePlayer->DroneMainUI = CreateWidget<UUserWidget>(GetWorld(), DronePlayer->DroneMainUIFactory);
        if (DronePlayer->DroneMainUI)
        {
            DronePlayer->DroneMainUI->AddToViewport(0);
            UE_LOG(LogTemp, Warning, TEXT("Drone UI created for player."));
        }
    }
}

