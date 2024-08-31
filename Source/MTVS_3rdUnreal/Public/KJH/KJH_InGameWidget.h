// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KJH/KJH_WidgetSystem.h"
#include "KJH_InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_InGameWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()
	
public:

	virtual bool Initialize();

////////// ���ε� ���� -----------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_CancelButton; // �ΰ��� UI -> �������� �����ϴ� ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_QuitButton; // �ΰ��� UI -> ������ �����ϴ� ��ư

	////////// ����� ������ �Լ� ���� -----------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void CancelPressed();

	UFUNCTION(BlueprintCallable)
	void QuitPressed();
};
