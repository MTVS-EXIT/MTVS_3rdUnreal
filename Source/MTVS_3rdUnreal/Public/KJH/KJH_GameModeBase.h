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
	// ������ ����
	AKJH_GameModeBase();


	// BeginPlay �Լ� ����
	virtual void BeginPlay() override; // BeginPlay �Լ��� �������̵��Ͽ� ����


	// �÷��̾ ���ӿ� ������ �� ȣ��Ǿ� ĳ���͸� �����ϴ� �Լ�
	virtual void RestartPlayer(AController* NewPlayer) override;

	class AKJH_PlayerState* PlayerState; // PlayerState ���� ����
};
