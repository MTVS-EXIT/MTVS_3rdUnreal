// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerState.h"
#include "Net/UnrealNetwork.h"

// ��Ʈ��ũ ���� ���� �Լ�
void AKJH_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsPersonCharacterSelected ������ �������� Ŭ���̾�Ʈ�� �����ǵ��� ����
    DOREPLIFETIME(AKJH_PlayerState, bIsPersonCharacterSelected);
}