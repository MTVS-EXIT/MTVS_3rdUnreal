// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HttpFwd.h"
#include "KJH_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKJH_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
////////// 생성자 & 초기화 함수 구간 ===================================================================
	AKJH_PlayerState();

////////// 변수 참조 구간 ===============================================================================================
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameState")
    bool bIsGameEnded; // 플레이 종료되었는지 여부 확인

	// 플레이어가 사람 캐릭터를 선택했는지 여부를 저장
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Character Selection")
	bool bIsPersonCharacterSelected = false;

	// 2) 소방관 플레이어 상태 변수 ------------------------------------------------------------
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 PersonState_SearchRoomCount = 0; // 소방관 탐색한 방 카운트

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 PersonState_ItemUsedCount = 0; // 소방관 물품 습득 카운트

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 PersonState_DamageCount = 0; // 소방관 부상 카운트

	// 3) 드론 플레이어 상태 변수 ------------------------------------------------------------
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 DroneState_DetectedCount = 0; // 드론 탐지 횟수 카운트

    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 DroneState_DetectedSafeCount = 0; // 안전 횟수 카운트

    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 DroneState_DetectedCautionCount = 0; // 주의 횟수 카운트

    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 DroneState_DetectedDangerCount = 0; // 위험 횟수 카운트


////////// 사용자 정의형 함수 구간 ===============================================================================================
	// 네트워크 복제 설정 함수 ------------------------------------------------------------------------------
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 소방관 이벤트 시 카운트 함수 -----------------------------------------------------------------------
    UFUNCTION(Server, Reliable)
    void IncrementPersonSearchRoomCount(); // 방 탐색 시 카운트

    UFUNCTION(Server, Reliable)
    void IncrementPersonItemUsedCount(); // 사물 활용 시 카운트

    UFUNCTION(Server, Reliable)
    void IncrementPersonDamageCount(); // 데미지 받을 시 카운트

	// 드론 이벤트 시 카운트 함수 -------------------------------------------------------------------------
    UFUNCTION(Server, Reliable)
    void IncrementDroneDetectedCount(); // 탐지 횟수 카운트 함수

    UFUNCTION(Server, Reliable)
    void IncrementDroneCategoryCount(int32 CategoryNum, int32 Num); // 카테고리별, 갯수를 인자로 받아 카운트 함수

	// 4) 백엔드 팀에게 전달하는 정보 관련 함수 ----------------------------------------------------------
	void SendDataToServer();
	void OnDataSendComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
