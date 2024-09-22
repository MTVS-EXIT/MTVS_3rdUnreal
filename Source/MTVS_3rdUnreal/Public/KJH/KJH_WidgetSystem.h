// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KJH_Interface.h"
#include "KJH_WidgetSystem.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_WidgetSystem : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetMyInterface(IKJH_Interface* Interface);

	UFUNCTION(BlueprintCallable)
	virtual void Setup(); // ServerUI를 Setup 하는 함수

	UFUNCTION(BlueprintCallable)
	virtual void Teardown(); // ServerUI를 다 사용했을 경우, 제거하는 함수

////////// 클래스 참조 구간 ------------------------------------------------------------
	class IKJH_Interface* MenuInterface;
};
