// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KJH_CharacterSelectWidget.h"
#include "KHS/KHS_DronePlayer.h"
#include "JSH/JSH_Player.h"
#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"
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

    // �Է� ���� ��� ���� �Լ� ����
    virtual void SetupInputComponent() override; // SetupInputComponent �Լ� ����

////////// ����� ������ �Լ� ���� ==================================================================================================================
 
     // 1) ĳ���� ���� UI ���� ---------------------------------------------------------------------------
    void ShowCharacterSelectWidget();  // ĳ���� ���� UI�� ǥ���ϴ� �Լ� (���� Ŭ���̾�Ʈ������ ����)

    UFUNCTION(Client, Reliable)
    void ClientShowCharacterSelectWidget(); // ���� -> Ŭ���̾�Ʈ ĳ���� ���� UI�� �����ִ� �Լ�

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpawnCharacterBasedOnSelection(bool bIsPersonSelected); // Ŭ���̾�Ʈ -> ���� ĳ���� ���� ��û �Լ�

    void SpawnCharacterBasedOnSelection(); // ĳ���� ���ÿ� ���� ĳ���͸� �����ϴ� �Լ�

    UFUNCTION(Client, Reliable)
    void Client_SetupDroneUI(); // ���� -> Ŭ���̾�Ʈ ��� UI�� �����ϴ� �Լ�

    // 2) InGameWidget UI ���� ---------------------------------------------------------------------------
    void ToggleInGameWidget(const FInputActionValue& Value);

    // 3) ĳ���� �������Ʈ �񵿱� �ε� ���� -------------------------------------------------------------


////////// ���� ���� ���� ============================================================================================================================
    bool bIsPersonCharacterSelected; // ĳ���� ���� ���� ���� (true: ���, false: ���)
    bool bIsInGameWidgetVisible; // InGameWidget ���ü� ���� ����

////////// TSubclass & class ���� ���� ================================================================================================================
    // 1) ĳ���� ���� �Է� Ű EnhancedInput ���� --------------------------------------------
    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputMappingContext* IMC_Common; // Mapping Context ����

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* IA_ToggleInGameWidget; // InputAction ����

    // 2) UI ���� ---------------------------------------------------------------------------
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) ����
    class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) ���� ����

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UKJH_InGameWidget> InGameWidgetFactory; // InGameWidget(UI) ����
    class UKJH_InGameWidget* InGameWidget; // InGameWidget(UI) ���� ����

    // 3) ĳ���� �������Ʈ ���� ------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // Player BP ����

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // Drone BP ����

    // 4) SpawnPoint �������Ʈ ���� ------------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> PersonSpawnPointClass; // PersonSpawnPoint BP ���� (���� �ڵ忡�� �±׷� ã���Ƿ� �ʼ��� �ƴ�)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> DroneSpawnPointClass; // DroneSpawnPoint BP ���� (���� �ڵ忡�� �±׷� ã���Ƿ� �ʼ��� �ƴ�)




};
