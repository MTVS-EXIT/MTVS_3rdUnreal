// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "KHS/KHS_DronePlayer.h"

void AKJH_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay���� ShowCharacterSelectWidget�� ȣ���Ͽ� Ŭ���̾�Ʈ������ UI�� �����ϵ��� ����
    if (IsLocalController()) // ���� Ŭ���̾�Ʈ�� ���� ����
    {
        ShowCharacterSelectWidget();
    }
}

void AKJH_PlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    // ĳ���Ͱ� Possess�� �Ŀ��� UI�� ǥ���ϵ��� ����
    if (IsLocalController()) // ���� Ŭ���̾�Ʈ�� ���� ����
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
    // ���� ��Ʈ�ѷ������� UI�� �����ϵ��� ����
    if (IsLocalPlayerController())
    {
        if (!CharacterSelectWidget && CharacterSelectWidgetFactory)
        {
            // ���� ����
            CharacterSelectWidget = CreateWidget<UKJH_CharacterSelectWidget>(this, CharacterSelectWidgetFactory);
            if (CharacterSelectWidget)
            {
                // ���� ���� �� ����Ʈ�� �߰�
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
    if (!HasAuthority()) // Ŭ���̾�Ʈ���� ����� ���
    {
        ServerSpawnCharacterBasedOnSelection(bIsPersonCharacterSelected);
        return;
    }

    // ���������� ���� ����
    TSubclassOf<APawn> ChosenCharacterClass = bIsPersonCharacterSelected
        ? BP_JSH_PlayerClass
        : BP_KHS_DronePlayerClass;

    // GameMode�� ���� ���� ��ġ ���
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
            // �⺻ ��ġ (�ʿ��� ��� ����)
            NewSpawnLocation = FVector(0.f, 0.f, 100.f);
            NewSpawnRotation = FRotator::ZeroRotator;
        }
    }
    else
    {
        // �⺻ ��ġ (�ʿ��� ��� ����)
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
        // ��Ʈ��ũ ����
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

