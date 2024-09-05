// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerState.h"
#include "Net/UnrealNetwork.h"

// 네트워크 복제 설정 함수
void AKJH_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsPersonCharacterSelected 변수가 서버에서 클라이언트로 복제되도록 설정
    DOREPLIFETIME(AKJH_PlayerState, bIsPersonCharacterSelected);
}