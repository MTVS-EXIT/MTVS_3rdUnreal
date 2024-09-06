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

// GameMode �����ڿ��� �ʱ� ����
AKJH_GameModeBase::AKJH_GameModeBase()
{
    // �÷��̾ ���� ���� �� �ڵ����� ���۵��� �ʰ� �����ڷ� ���۵ǰ� ����
    bStartPlayersAsSpectators = true;
}

void AKJH_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // �⺻������ RestartPlayer�� ȣ������ �ʰ�, Ư�� �̺�Ʈ�� ���ǿ����� ȣ��ǵ��� ����
    // ���� ���, �÷��̾ ĳ���͸� ������ �� ȣ��ǵ��� ��

    // ĳ���� ���� �Ŀ��� RestartPlayer�� ȣ��ǵ��� ����
    // ���� ĳ���� ���� �������� GameMode�� RestartPlayer�� ȣ���ϵ��� ������
    // �̰� GameInstance�� OnCharacterSelected ���� ó����.
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

    // ���� ������ �÷��̾�� ���� ���� ����
    if (NewPlayer)
    {
        APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
        if (PlayerController)
        {
            // ���� ���� ������ ĳ���� ���� �������� �ʿ�
            PlayerController->ChangeState(NAME_Spectating);
        }
    }

    // �������� ���� ����
    if (HasAuthority()) // ���������� ����
    {
        ShowCharacterSelectWidget(NewPlayer);
    }
    // Ŭ���̾�Ʈ���� Character Select UI�� ǥ���϶�� ��û
    Multicast_ShowCharacterSelectWidget(NewPlayer);
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);

    if (!NewPlayer) return;

    // �÷��̾� ���¸� ������
    AKJH_PlayerState* PlayerState = NewPlayer->GetPlayerState<AKJH_PlayerState>();

    // �÷��̾� ��Ʈ�ѷ� ��������
    APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
    if (!PlayerController) return;

    // ���� ��忡�� �÷��� ���� ��ȯ�� ���� �۵��ϰ� ����
    if (PlayerState)
    {
        // �� �÷��̾��� PlayerState�� ���� ĳ���� Ŭ������ ����
        TSubclassOf<APawn> ChosenCharacterClass = PlayerState->bIsPersonCharacterSelected
            ? BP_JSH_PlayerClass // true �� ����� ���õǾ��ٰ� �����ϰ� ��� BPŬ������ ����
            : BP_KHS_DronePlayerClass; // false�� ����� ���õǾ��ٰ� �����ϰ� ��� BPŬ������ ����

        // ����� �޽����� ���õ� ĳ���� Ŭ���� ���
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue,
            FString::Printf(TEXT("Chosen Character Class: %s"), *ChosenCharacterClass->GetName()));


        // ���� �Ķ���� ����
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = NewPlayer;
        SpawnParams.Instigator = NewPlayer->GetPawn();

        // ���� ��ġ�� ȸ�� �� ����
        FVector SpawnLocation = FVector(0.f, 0.f, 100.f); // �⺻ ���� ��ġ ���� (�ʿ信 ���� ����)
        FRotator SpawnRotation = FRotator::ZeroRotator; // �⺻ ȸ�� �� ����

        // ���� �� ����� �޽��� �߰�
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Attempting to spawn character..."));

        // ���ο� ĳ���� ����
        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewPawn)
        {
            // �÷��̾ �� ĳ���͸� �����ϵ��� ����
            PlayerController->Possess(NewPawn);

            // ������ ��忡�� �÷��� ���� ��ȯ
            PlayerController->ChangeState(NAME_Playing);

            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Character successfully spawned!"));
            UE_LOG(LogTemp,Warning, TEXT("Character successfully spawned!"));
        }

        else
        {
            // PlayerState�� ���� ��� �⺻ ���� ����
            Super::RestartPlayer(NewPlayer);
        }
    }
}

////////// RPC �Լ� ���� ------------------------------------------------------------------------------------------------
void AKJH_GameModeBase::Multicast_ShowCharacterSelectWidget_Implementation(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid for showing CharacterSelectWidget."));
        return;
    }

    // �� �Լ��� Ŭ���̾�Ʈ������ �����, ���� ������ ���⼭ UI ���� ����
    if (!HasAuthority()) // Ŭ���̾�Ʈ������ ����
    {
        ShowCharacterSelectWidget(PlayerController);
    }
}

////////// ����� ������ �Լ� ���� ---------------------------------------------------------------------------------------------

// ĳ���� ���� UI ���� �Լ�
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
        // ���������� �� �÷��̾�� ���� ���� �� ���� (CharacterSelectWidget ��� ������ ������� ����)
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
