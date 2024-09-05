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


////////// TSubclass & class 참조 구간 -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_ServerWidget> ServerWidgetFactory; // ServerWidget(UI) 공장
	class UKJH_ServerWidget* ServerWidget; // ServerWidget(UI) 참조 선언

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) 공장
	class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) 참조 선언
	
	class AKJH_PlayerState* PlayerState; // PlayerState 참조 선언

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes") // Player와 Drone BP 클래스 참조
	TSubclassOf<class AJSH_Player> BP_JSH_PlayerClass; // 수정된 부분

	UPROPERTY(EditDefaultsOnly, Category = "Character Classes")
	TSubclassOf<class AKHS_DronePlayer> BP_KHS_DronePlayerClass; // 수정된 부분
};
