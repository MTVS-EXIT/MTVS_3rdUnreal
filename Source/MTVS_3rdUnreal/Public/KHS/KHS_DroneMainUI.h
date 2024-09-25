// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KHS_DroneMainUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKHS_DroneMainUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//드론 고도계 처리
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HeightText;

	//AI 처리결과 출력
	UPROPERTY(meta = (BindWidget))
	class UImage* AIImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AIChatResult;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* CaputreUIAnim;

	void PlayCaptureAnim();
};
