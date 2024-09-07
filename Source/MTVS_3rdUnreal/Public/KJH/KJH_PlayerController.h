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

    // 초기화 함수, BeginPlay에서 호출
    virtual void BeginPlay() override;

    // 캐릭터가 Possess될 때 호출되는 함수
    virtual void OnPossess(APawn* aPawn) override;

    // 캐릭터 선택 UI를 표시하는 함수
    void ShowCharacterSelectWidget();

    UFUNCTION(Client, Reliable)
    void ClientShowCharacterSelectWidget();

    // 캐릭터 선택에 따라 캐릭터를 스폰하는 함수
    void SpawnCharacterBasedOnSelection();

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpawnCharacterBasedOnSelection(bool bIsPersonSelected);


    // 캐릭터 선택 상태 변수 (true: 사람, false: 드론)
    bool bIsPersonCharacterSelected;

    // 캐릭터 선택 위젯
    UPROPERTY()
    UKJH_CharacterSelectWidget* CharacterSelectWidget;

    // 위젯 팩토리 (블루프린트에서 설정 가능)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes") // Player와 Drone BP 클래스 참조
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // 수정된 부분

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // 수정된 부분

};
