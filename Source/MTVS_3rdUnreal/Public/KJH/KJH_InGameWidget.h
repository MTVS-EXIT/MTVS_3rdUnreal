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

////////// �ʱ�ȭ �Լ� ���� ===================================================================
	virtual bool Initialize(); // UserWidget �ʱ�ȭ �Լ�

	virtual void Setup() override;
////////// ���ε� ���� ========================================================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InGameMenuAnim; // �ΰ��� UI ȣ�� �� �����ϴ� �ִϸ��̼� ����

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_CancelButton; // �ΰ��� UI -> �������� �����ϴ� ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_QuitButton; // �ΰ��� UI -> ������ �����Ͽ� ���θ޴��� �̵��ϴ� ��ư

////////// ����� ������ �Լ� ���� ���� ========================================================

	UFUNCTION(BlueprintCallable)
	void CancelPressed();

	UFUNCTION(BlueprintCallable)
	void QuitPressed();
};
