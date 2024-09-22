// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KJH/KJH_WidgetSystem.h"
#include "KJH_LoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_LoadingWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()
	

public:

////////// �ʱ�ȭ �Լ� ���� ===================================================================
virtual bool Initialize(); // UserWidget �ʱ�ȭ �Լ�
virtual void Setup() override;
virtual void Teardown() override;

////////// UI ���ε� ���� ==============================================================================
	// 1) �޴� ü���� ���� ---------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // �� �޴��� ��ȯ��ų �� �ִ� Menu Switcher

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* CommonLoadingMenu; // ���� �ε� �޴� ����

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowCommonLoadingAnim; // ���� �ε� �޴� Show �ִϸ��̼�

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideCommonLoadingAnim; // ���� �ε� �޴� Hide �ִϸ��̼�
};
