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

    // �ʿ��� ��� �߰����� �ʱ�ȭ �۾��� ���⿡ ����
    // ��: �÷��̾� ���� �ʱ�ȭ, ���� ���� ������Ʈ ��
}



void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);
}

////////// RPC �Լ� ���� ------------------------------------------------------------------------------------------------


////////// ����� ������ �Լ� ���� ---------------------------------------------------------------------------------------------

