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

    // PlayerController���� ĳ���� ���� UI ����
    AKJH_PlayerController* KJHController = Cast<AKJH_PlayerController>(NewPlayer);
    if (KJHController)
    {
        KJHController->ShowCharacterSelectWidget();
    }
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
 
    // �� �̻� ĳ���� ���� ������ �������� �ʰ� �⺻ ��ɸ� ����
    Super::RestartPlayer(NewPlayer);
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
    if (PlayerController->IsLocalController()) // Ŭ���̾�Ʈ������ ����
    {
        // PlayerController���� ShowCharacterSelectWidget ȣ��
        AKJH_PlayerController* KJHController = Cast<AKJH_PlayerController>(PlayerController);
        if (KJHController)
        {
            KJHController->ShowCharacterSelectWidget();
        }
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
