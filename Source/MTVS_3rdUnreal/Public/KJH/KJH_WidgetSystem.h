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
	virtual void Setup(); // ServerUI�� Setup �ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	virtual void Teardown(); // ServerUI�� �� ������� ���, �����ϴ� �Լ�

////////// Ŭ���� ���� ���� ------------------------------------------------------------
	class IKJH_Interface* MenuInterface;
};
