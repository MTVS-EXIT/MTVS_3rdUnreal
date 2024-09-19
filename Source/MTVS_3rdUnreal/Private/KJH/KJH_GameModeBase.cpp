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

    // BeginPlay���� ������ NPC ����
    OnMySpawnRescueNPC();
}

////////// Login ���� �Լ� ���� (PreLogin -> Login -> PostLogin ������ ȣ��) ============================================================
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

    // �ʿ��� ��� �߰����� �ʱ�ȭ �۾��� ���⿡ ����
    // ��: �÷��̾� ���� �ʱ�ȭ, ���� ���� ������Ʈ ��

    ++NumberOfPlayers; // �÷��̾ �����Ͽ����Ƿ� ����

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
    --NumberOfPlayers; // �÷��̾ �����Ͽ����Ƿ� ����
}

////////// �÷��̾ ���ӿ� ������ �� ȣ��Ǿ� ĳ���͸� �����ϴ� �Լ� ========================================================
void AKJH_GameModeBase::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);
}

////////// ����� ������ �Լ� ���� ========================================================
void AKJH_GameModeBase::OnMySpawnRescueNPC()
{
    // ������ NPC �������Ʈ Ŭ������ �����ϴ��� Ȯ��
    if (BP_RescueNPCClass)
    {
        // ������ NPC�� ������ ���� ����Ʈ�� �±׷� ã��
        TArray<AActor*> FoundNPCSpawnPoints;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RescueNPCSpawnPoint"), FoundNPCSpawnPoints);

        if (FoundNPCSpawnPoints.Num() > 0)
        {
            // �������� ���� ����Ʈ ����
            int32 RandomIndex = FMath::RandRange(0, FoundNPCSpawnPoints.Num() - 1);
            AActor* SelectedSpawnPoint = FoundNPCSpawnPoints[RandomIndex];
            // AActor* SelectedSpawnPoint = FoundNPCSpawnPoints[0]; ù��°�� ã�� ����Ʈ�� ��������Ʈ�� �����Ѵ�. �ÿ��� Ȱ��ȭ�� ��.
            FVector SpawnLocation = SelectedSpawnPoint->GetActorLocation();
            FRotator SpawnRotation = SelectedSpawnPoint->GetActorRotation();

            // ���� �Ķ����(��Ģ) ����
            FActorSpawnParameters SpawnParams;

            // �� ���͸� �����ϰ� �ϴ� ��ü(����)�� �����ΰ�?
            // NPC ������ ���ʸ� ���Ӹ��� ����
            SpawnParams.Owner = this;

            // ���Ͱ� ������ �� �浹�� �Ͼ�� ��� �� ���ΰ�?
            // �浹�� �߻��ϸ� ��ġ�� ���� �����Ͽ� �����ϰ�, ������ �Ұ����ϴ��� ������ ����
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // NPC ����
            AActor* SpawnedNPC = GetWorld()->SpawnActor<AActor>(BP_RescueNPCClass, SpawnLocation, SpawnRotation, SpawnParams);
        }
    }
}


