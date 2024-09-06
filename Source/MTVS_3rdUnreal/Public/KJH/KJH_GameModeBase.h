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
	// 생성자 선언
	AKJH_GameModeBase();


	// BeginPlay 함수 선언
	virtual void BeginPlay() override; // BeginPlay 함수를 오버라이드하여 선언

	virtual void PreLogin(const FString& Options, const FString& Address, 
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어가 게임에 참여할 때 호출되어 캐릭터를 스폰하는 함수
	virtual void RestartPlayer(AController* NewPlayer) override;


////////// RPC 함수 구간 ------------------------------------------------------------------------------------------------------



////////// 사용자 정의형 함수 구간 ---------------------------------------------------------------------------------------------

// 캐릭터 선택 구간 //
void ShowCharacterSelectWidget(APlayerController* PlayerController); // 캐릭터 선택 UI 생성

UFUNCTION(NetMulticast, Reliable) // 서버가 모든 클라이언트에게 동일한 명령을 전달하는 것이므로, 클라이언트 입장에서는 데이터를 검증할 필요가 없음.
								  // 이미 신뢰가 있는 정보이므로, Validate 없이 Implementation만 구현
void Multicast_ShowCharacterSelectWidget(APlayerController* PlayerController); // 캐릭터 선택 UI 생성 멀티캐스트
void Multicast_ShowCharacterSelectWidget_Implementation(APlayerController* PlayerController); // 캐릭터 선택 UI 생성 멀티캐스트 구현부

////////// TSubclass & class 참조 구간 -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) 공장

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes") // Player와 Drone BP 클래스 참조
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // 수정된 부분

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // 수정된 부분




};
