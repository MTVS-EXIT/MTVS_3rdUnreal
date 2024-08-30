// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KHS_DroneAIVisionUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKHS_DroneAIVisionUI : public UUserWidget
{
	GENERATED_BODY()
	

public:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* PercentText;

	void SetPercentBlock();

	float CurrentTime = 0;

};
