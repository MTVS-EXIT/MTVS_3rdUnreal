// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KJH_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UKJH_GameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init();


	UFUNCTION(Exec)
	void Host();
};
