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
#include "KJH/KJH_ResultWidget.h"

// GameMode 생성자에서 초기 설정
AKJH_GameModeBase::AKJH_GameModeBase()
{
	// 플레이어가 게임 시작 시 자동으로 시작되지 않고 관전자로 시작되게 설정
	bStartPlayersAsSpectators = true;

	// 처음엔 게임이 종료되지 않은 상태로 설정
	bIsGameEnded = false;
}

void AKJH_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 게임시작 시 피구조자 NPC 스폰
	OnMySpawnRescueNPC();
}

////////// Login 관련 함수 선언 (PreLogin -> Login -> PostLogin 순서로 호출) ============================================================
void AKJH_GameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	FString Value;
	UGameplayStatics::ParseOption(TEXT("PlayerSelected"), Value);
	UE_LOG(LogTemp, Warning, TEXT("PreLogin"));
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

////////// 사용자 정의형 함수 구간 ============================================================================================
// 피구조자 NPC를 랜덤 스폰하는 함수
void AKJH_GameModeBase::OnMySpawnRescueNPC()
{
	// 스폰할 NPC 블루프린트 클래스가 존재하는지 확인
	if (BP_RescueNPCClass)
	{
		// 구조자 NPC를 스폰할 스폰 포인트를 태그로 찾기
		TArray<AActor*> FoundNPCSpawnPoints;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("RescueNPCSpawnPoint"), FoundNPCSpawnPoints);

		if (FoundNPCSpawnPoints.Num() > 0)
		{
			// 랜덤으로 스폰 포인트 선택
			int32 RandomIndex = FMath::RandRange(0, FoundNPCSpawnPoints.Num() - 1);
			AActor* SelectedSpawnPoint = FoundNPCSpawnPoints[RandomIndex];
			// AActor* SelectedSpawnPoint = FoundNPCSpawnPoints[0]; 첫번째로 찾은 포인트를 스폰포인트로 지정한다. 시연때 활성화할 것.
			FVector SpawnLocation = SelectedSpawnPoint->GetActorLocation();
			FRotator SpawnRotation = SelectedSpawnPoint->GetActorRotation();

			// 스폰 파라미터(규칙) 설정
			FActorSpawnParameters SpawnParams;

			// 이 액터를 스폰하게 하는 주체(오너)는 누구인가?
			// NPC 액터의 오너를 게임모드로 설정
			SpawnParams.Owner = this;

			// 액터가 스폰될 때 충돌이 일어나면 어떻게 할 것인가?
			// 충돌이 발생하면 위치를 조금 조정하여 스폰하고, 조정이 불가능하더라도 강제로 스폰
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// NPC 생성
			AActor* SpawnedNPC = GetWorld()->SpawnActor<AActor>(BP_RescueNPCClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

// 플레이 종료를 알리는 함수
void AKJH_GameModeBase::Multicast_TriggerGameEnd_Implementation()
{
	if (bIsGameEnded)
	return;

	bIsGameEnded = true;

	// 로컬 플레이어 컨트롤러에 대해서만 위젯 생성
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->IsLocalController() && ResultWidgetClass)
	{
		UKJH_ResultWidget* ResultWidget = CreateWidget<UKJH_ResultWidget>(PC, ResultWidgetClass);
		if (ResultWidget)
		{
			ResultWidget->Setup();
			ResultWidget->PlayResultAnimations(); // 애니메이션 시작
		}
	}
}

