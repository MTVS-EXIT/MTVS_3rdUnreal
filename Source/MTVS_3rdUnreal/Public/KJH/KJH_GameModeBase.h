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

	virtual void PreLogin(const FString& Options, const FString& Address, 
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	// �÷��̾ ���ӿ� ������ �� ȣ��Ǿ� ĳ���͸� �����ϴ� �Լ�
	virtual void RestartPlayer(AController* NewPlayer) override;


////////// TSubclass & class ���� ���� -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_ServerWidget> ServerWidgetFactory; // ServerWidget(UI) ����
	class UKJH_ServerWidget* ServerWidget; // ServerWidget(UI) ���� ����

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) ����
	class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) ���� ����
	
	class AKJH_PlayerState* PlayerState; // PlayerState ���� ����

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes") // Player�� Drone BP Ŭ���� ����
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // ������ �κ�

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // ������ �κ�
};
