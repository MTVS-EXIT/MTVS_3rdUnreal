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
	// GameMode�� ������ ���� =================================================================================================
	AKJH_GameModeBase();

	// GameMode�� BeginPlay ���� ==============================================================================================
	virtual void BeginPlay() override;

	// Login ���� �Լ� ���� ===================================================================================================
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

////////// �÷��̾ ���ӿ� ������ �� ȣ��Ǿ� ĳ���͸� �����ϴ� �Լ� ========================================================
	virtual void RestartPlayer(AController* NewPlayer) override;


////////// ���� ���� ���� ======================================================================================================
	
	uint32 NumberOfPlayers = 0; // ������ �÷��̾��� ��


////////// ����� ������ �Լ� ���� ============================================================================================



////////// TSubclass & class ���� ���� =======================================================================================


};
