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

	//��� ���� ó��
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HeightText;

	//AI ó����� ���
	UPROPERTY(meta = (BindWidget))
	class UImage* AIImage;

};
