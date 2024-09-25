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

////////// 생성자 & 초기화 함수 구간 ================================================================================================================

    virtual void BeginPlay() override; // BeginPlay 초기화 함수
    virtual void OnPossess(APawn* aPawn) override; // 캐릭터가 Possess될 때 호출되는 함수

    // 입력 구성 요소 설정 함수 선언
    virtual void SetupInputComponent() override; // SetupInputComponent 함수 선언

////////// 사용자 정의형 함수 구간 ==================================================================================================================
 
     // 1) 캐릭터 선택 UI 관련 ---------------------------------------------------------------------------
    void ShowCharacterSelectWidget();  // 캐릭터 선택 UI를 표시하는 함수 (로컬 클라이언트에서만 실행)

    UFUNCTION(Client, Reliable)
    void ClientShowCharacterSelectWidget(); // 서버 -> 클라이언트 캐릭터 선택 UI를 보여주는 함수

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpawnCharacterBasedOnSelection(bool bIsPersonSelected); // 클라이언트 -> 서버 캐릭터 스폰 요청 함수

    void SpawnCharacterBasedOnSelection(); // 캐릭터 선택에 따라 캐릭터를 스폰하는 함수

    // 2) 드론 & 플레이어 고유 UI 생성 관련 -------------------------------------------------------------
    UFUNCTION(Server, Reliable)
    void Server_NotifyPawnPossessed(APawn* aPawn); // ServerRPC를 통해 Possess 정보를 확인하고 
                                                   // 적절한 UI 설정을 트리거하는 함수

    UFUNCTION(Client, Reliable)
    void Client_SetupDroneUI(); // 드론 UI를 설정하는 ClientRPC

    UFUNCTION(Client, Reliable)
    void Client_SetupPersonUI(); // 소방관 UI를 설정하는 ClientRPC

    // 3) InGameWidget UI 관련 ---------------------------------------------------------------------------
    void ToggleInGameWidget(const FInputActionValue& Value);



////////// 전역 변수 구간 ============================================================================================================================
    bool bIsPersonCharacterSelected; // 캐릭터 선택 상태 변수 (true: 사람, false: 드론)
    bool bIsInGameWidgetVisible; // InGameWidget 가시성 상태 변수

////////// TSubclass & class 참조 구간 ================================================================================================================
    // 1) 캐릭터 공통 입력 키 EnhancedInput 관련 --------------------------------------------
    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputMappingContext* IMC_Common; // Mapping Context 참조

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* IA_ToggleInGameWidget; // InputAction 참조

    // 2) UI 관련 ---------------------------------------------------------------------------
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) 공장
    class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) 참조 선언

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UKJH_InGameWidget> InGameWidgetFactory; // InGameWidget(UI) 공장
    class UKJH_InGameWidget* InGameWidget; // InGameWidget(UI) 참조 선언

   UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UKHS_DroneMainUI> DroneWidgetFactory; // DroneWidget(UI) 공장
    class UKHS_DroneMainUI* DroneWidget; // DroneWidget(UI) 참조 선언

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UJSH_PlayerMainUI> PersonWidgetFactory; // PersonWidget(UI) 공장
    class UJSH_PlayerMainUI* PersonWidget; // PersonWidget(UI) 참조 선언

    // 3) 캐릭터 블루프린트 관련 ------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // Player BP 참조

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // Drone BP 참조

    // 4) SpawnPoint 블루프린트 관련 ------------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> PersonSpawnPointClass; // PersonSpawnPoint BP 참조 (현재 코드에선 태그로 찾으므로 필수는 아님)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
    TSubclassOf<AActor> DroneSpawnPointClass; // DroneSpawnPoint BP 참조 (현재 코드에선 태그로 찾으므로 필수는 아님)

};
