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


////////// RPC �Լ� ���� ------------------------------------------------------------------------------------------------------



////////// ����� ������ �Լ� ���� ---------------------------------------------------------------------------------------------

// ĳ���� ���� ���� //
void ShowCharacterSelectWidget(APlayerController* PlayerController); // ĳ���� ���� UI ����

UFUNCTION(NetMulticast, Reliable) // ������ ��� Ŭ���̾�Ʈ���� ������ ����� �����ϴ� ���̹Ƿ�, Ŭ���̾�Ʈ ���忡���� �����͸� ������ �ʿ䰡 ����.
								  // �̹� �ŷڰ� �ִ� �����̹Ƿ�, Validate ���� Implementation�� ����
void Multicast_ShowCharacterSelectWidget(APlayerController* PlayerController); // ĳ���� ���� UI ���� ��Ƽĳ��Ʈ
void Multicast_ShowCharacterSelectWidget_Implementation(APlayerController* PlayerController); // ĳ���� ���� UI ���� ��Ƽĳ��Ʈ ������

////////// TSubclass & class ���� ���� -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) ����

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes") // Player�� Drone BP Ŭ���� ����
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // ������ �κ�

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // ������ �κ�




};
