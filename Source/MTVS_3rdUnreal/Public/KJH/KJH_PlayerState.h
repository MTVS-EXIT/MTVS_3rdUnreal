// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KJH_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKJH_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// ��Ʈ��ũ ���� ���� �Լ�
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


    // �÷��̾ ��� ĳ���͸� �����ߴ��� ���θ� ����
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Character Selection")
	bool bIsPersonCharacterSelected = false;

};
