// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KJH_CharacterSelectWidget.h"
#include "KHS/KHS_DronePlayer.h"
#include "JSH/JSH_Player.h"
#include "GameFramework/Actor.h"
#include "KJH_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKJH_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

////////// ������ & �ʱ�ȭ �Լ� ���� ================================================================================================================

    virtual void BeginPlay() override; // BeginPlay �ʱ�ȭ �Լ�
    virtual void OnPossess(APawn* aPawn) override; // ĳ���Ͱ� Possess�� �� ȣ��Ǵ� �Լ�

////////// ����� ������ �Լ� ���� - ĳ���� ���� UI ���� =============================================================================================
   
    void ShowCharacterSelectWidget();  // ĳ���� ���� UI�� ǥ���ϴ� �Լ� (���� Ŭ���̾�Ʈ������ ����)

    UFUNCTION(Client, Reliable)
    void ClientShowCharacterSelectWidget(); // ���� -> Ŭ���̾�Ʈ ĳ���� ���� UI�� �����ִ� �Լ�

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpawnCharacterBasedOnSelection(bool bIsPersonSelected); // Ŭ���̾�Ʈ -> ���� ĳ���� ���� ��û �Լ�

    void SpawnCharacterBasedOnSelection(); // ĳ���� ���ÿ� ���� ĳ���͸� �����ϴ� �Լ�

    UFUNCTION(Client, Reliable)
    void Client_SetupDroneUI(); // ���� -> Ŭ���̾�Ʈ ��� UI�� �����ϴ� �Լ�

////////// ���� ���� ���� ============================================================================================================================
    bool bIsPersonCharacterSelected; // ĳ���� ���� ���� ���� (true: ���, false: ���)

////////// TSubclass & class ���� ���� ================================================================================================================
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) ����

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) ���� ����

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // Player BP ����

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // Drone BP ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> PersonSpawnPointClass; // PersonSpawnPoint BP ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> DroneSpawnPointClass; // DroneSpawnPoint BP ����
};
