// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KJH_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKJH_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// 네트워크 복제 설정 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


    // 플레이어가 사람 캐릭터를 선택했는지 여부를 저장
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Character Selection")
	bool bIsPersonCharacterSelected = false;

};
