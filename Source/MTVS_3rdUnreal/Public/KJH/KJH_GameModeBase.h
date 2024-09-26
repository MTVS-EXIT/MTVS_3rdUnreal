// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KJH_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKJH_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

///////// GameMode의 생성자 선언 =================================================================================================
	AKJH_GameModeBase(); // 생성자

///////// GameMode의 BeginPlay 선언 ==============================================================================================
	virtual void BeginPlay() override; // BeginPlay

///////// Login 관련 함수 선언 ===================================================================================================
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

////////// 플레이어가 게임에 참여할 때 호출되어 캐릭터를 스폰하는 함수 ===========================================================
	virtual void RestartPlayer(AController* NewPlayer) override;

////////// 변수 참조 구간 ========================================================================================================
	uint32 NumberOfPlayers = 0; // 접속한 플레이어의 수

////////// TSubclass & class 참조 구간 ===========================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	TSubclassOf<class AActor> BP_RescueNPCClass; // 구조자(NPC) BP 참조

    //UPROPERTY(EditDefaultsOnly, Category = "UI")
    //TSubclassOf<class UKJH_WidgetSystem> ResultWidgetClass; // 플레이 결과 UI 클래스 참조

////////// 변수 참조 구간 ===============================================================================================
	//bool bIsGameEnded; // 플레이 종료되었는지 여부 확인

////////// 사용자 정의형 함수 구간 ===============================================================================================
	void OnMySpawnRescueNPC();

    //UFUNCTION(NetMulticast, Reliable)
    //void Multicast_TriggerGameEnd(); // 플레이 종료를 알리는 RPC 함수
};
