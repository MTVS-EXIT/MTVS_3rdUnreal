// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KJH_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKJH_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MTVS_3RDUNREAL_API IKJH_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Host(FString ServerName) = 0; // = 0 을 하면 비어져 있는 구현을 가지고 있다는 뜻. (순수가상) 실제로는 GameInstance에서 구현해줄거다.

	virtual void Join(uint32 Index) = 0; // 위와 같은 로직
	virtual void LoadServerWidgetMap() = 0; // 위와 같은 로직
	virtual void RefreshServerList() = 0; // 위와 같은 로직

};
