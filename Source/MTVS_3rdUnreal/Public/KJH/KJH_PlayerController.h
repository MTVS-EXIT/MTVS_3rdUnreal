// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KJH_CharacterSelectWidget.h"
#include "KHS/KHS_DronePlayer.h"
#include "JSH/JSH_Player.h"
#include "KJH_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKJH_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

    // �ʱ�ȭ �Լ�, BeginPlay���� ȣ��
    virtual void BeginPlay() override;

    // ĳ���Ͱ� Possess�� �� ȣ��Ǵ� �Լ�
    virtual void OnPossess(APawn* aPawn) override;

    // ĳ���� ���� UI�� ǥ���ϴ� �Լ�
    void ShowCharacterSelectWidget();

    UFUNCTION(Client, Reliable)
    void ClientShowCharacterSelectWidget();

    // ĳ���� ���ÿ� ���� ĳ���͸� �����ϴ� �Լ�
    void SpawnCharacterBasedOnSelection();

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpawnCharacterBasedOnSelection(bool bIsPersonSelected);


    // ĳ���� ���� ���� ���� (true: ���, false: ���)
    bool bIsPersonCharacterSelected;

    // ĳ���� ���� ����
    UPROPERTY()
    UKJH_CharacterSelectWidget* CharacterSelectWidget;

    // ���� ���丮 (�������Ʈ���� ���� ����)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes") // Player�� Drone BP Ŭ���� ����
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // ������ �κ�

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // ������ �κ�

};
