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

////////// 생성자 & 초기화 함수 구간 ================================================================================================================

    virtual void BeginPlay() override; // BeginPlay 초기화 함수
    virtual void OnPossess(APawn* aPawn) override; // 캐릭터가 Possess될 때 호출되는 함수

////////// 사용자 정의형 함수 구간 - 캐릭터 선택 UI 관련 =============================================================================================
   
    void ShowCharacterSelectWidget();  // 캐릭터 선택 UI를 표시하는 함수 (로컬 클라이언트에서만 실행)

    UFUNCTION(Client, Reliable)
    void ClientShowCharacterSelectWidget(); // 서버 -> 클라이언트 캐릭터 선택 UI를 보여주는 함수

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpawnCharacterBasedOnSelection(bool bIsPersonSelected); // 클라이언트 -> 서버 캐릭터 스폰 요청 함수

    void SpawnCharacterBasedOnSelection(); // 캐릭터 선택에 따라 캐릭터를 스폰하는 함수

    UFUNCTION(Client, Reliable)
    void Client_SetupDroneUI(); // 서버 -> 클라이언트 드론 UI를 설정하는 함수

////////// 전역 변수 구간 ============================================================================================================================
    bool bIsPersonCharacterSelected; // 캐릭터 선택 상태 변수 (true: 사람, false: 드론)

////////// TSubclass & class 참조 구간 ================================================================================================================
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) 공장

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) 참조 선언

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // Player BP 참조

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // Drone BP 참조

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> PersonSpawnPointClass; // PersonSpawnPoint BP 참조

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> DroneSpawnPointClass; // DroneSpawnPoint BP 참조
};
