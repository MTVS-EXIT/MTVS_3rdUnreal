// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerState.h"
#include "Net/UnrealNetwork.h"


////////// 사용자 정의형 함수 구간 - 네트워크 복제 설정 함수 ----------------------------------------------------------------------------------------------------------------
// 네트워크 복제 설정 함수
void AKJH_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsPersonCharacterSelected 변수가 서버에서 클라이언트로 복제되도록 설정
    DOREPLIFETIME(AKJH_PlayerState, bIsPersonCharacterSelected);

    DOREPLIFETIME(AKJH_PlayerState, bIsGameEnded);
    DOREPLIFETIME(AKJH_PlayerState, PersonState_SearchRoomCount);
    DOREPLIFETIME(AKJH_PlayerState, PersonState_ItemUsedCount);
    DOREPLIFETIME(AKJH_PlayerState, PersonState_DamageCount);
    DOREPLIFETIME(AKJH_PlayerState, DroneState_SearchRoomCount);
    DOREPLIFETIME(AKJH_PlayerState, DroneState_DetectedCount);
    DOREPLIFETIME(AKJH_PlayerState, DroneState_DamageCount);
}

////////// 사용자 정의형 함수 구간 - 소방관 이벤트 시 카운트 함수 ===============================================================================
void AKJH_PlayerState::IncrementPersonSearchRoomCount_Implementation()
{
    PersonState_SearchRoomCount++;
}

void AKJH_PlayerState::IncrementPersonItemUsedCount_Implementation()
{
    PersonState_ItemUsedCount++;
}

void AKJH_PlayerState::IncrementPersonDamageCount_Implementation()
{
    PersonState_DamageCount++;
}

////////// 사용자 정의형 함수 구간 - 드론 이벤트 시 카운트 함수 ===============================================================================
void AKJH_PlayerState::IncrementDroneSearchRoomCount_Implementation()
{
    DroneState_SearchRoomCount++;
}

void AKJH_PlayerState::IncrementDroneDetectedCount_Implementation()
{
    DroneState_DetectedCount++;
}

void AKJH_PlayerState::IncrementDroneDamageCount_Implementation()
{
    DroneState_DamageCount++;
}
