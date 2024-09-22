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

////////// 초기화 함수 구간 ===================================================================
virtual bool Initialize(); // UserWidget 초기화 함수
virtual void Setup() override;
virtual void Teardown() override;

////////// UI 바인딩 구간 ==============================================================================
	// 1) 메뉴 체인지 관련 ---------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 메뉴로 전환시킬 수 있는 Menu Switcher

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* CommonLoadingMenu; // 공통 로딩 메뉴 위젯

	// 2) 위젯 애니메이션 관련 -----------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowCommonLoadingAnim; // 공통 로딩 메뉴 Show 애니메이션

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideCommonLoadingAnim; // 공통 로딩 메뉴 Hide 애니메이션
};
