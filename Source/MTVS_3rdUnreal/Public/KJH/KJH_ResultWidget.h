// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KJH/KJH_WidgetSystem.h"
#include "KJH_ResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_ResultWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()
	
public :
////////// 생성자 & 초기화 함수 구간 ===================================================================
	virtual bool Initialize(); // 초기화 함수

////////// UI 바인딩 구간 ==============================================================================
	// 1) 메뉴 체인지 관련 -----------------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 메뉴로 전환시킬 수 있는 Menu Switcher

	// 2) 공통 메뉴 관련 -----------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* CommonResultMenu_QuitButton; // 플레이를 종료하여 메인메뉴로 이동하는 버튼

	// 3) 소방관 플레이어 메뉴 관련 ------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* PersonResultMenu; // 소방관 플레이어 결과 메뉴 위젯

	UPROPERTY(meta = (BindWidget))
	class UButton* PersonResultMenu_DroneResultButton; // 드론 플레이어 결과 메뉴로 전환 버튼

	// 4) 드론 플레이어 메뉴 관련 --------------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* DroneResultMenu; // 드론 플레이어 결과 메뉴 위젯

	UPROPERTY(meta = (BindWidget))
	class UButton* DroneResultMenu_PersonResultButton; // 소방관 플레이어 결과 메뉴로 전환 버튼
	
	// 5) 위젯 애니메이션 관련 -----------------------------------------------------------------------
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowRescueAnim; // 구조 시 애니메이션

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowCommonResultAnim; // 공통 결과 메뉴 애니메이션

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowPersonResultAnim; // 소방관 결과 메뉴 애니메이션

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowDroneResultAnim; // 드론 결과 메뉴 애니메이션



////////// 사용자 정의형 함수 구간 ==============================================================================
	// 1) 버튼 함수 관련 ----------------------------------------------------------------------------------------
	UFUNCTION()
	void QuitPressed(); // 플레이 종료 함수

	UFUNCTION()
	void SwitchToPersonResultMenu(); // 소방관 결과 메뉴로 전환 함수

	UFUNCTION()
	void SwitchToDroneResultMenu(); // 드론 결과 메뉴로 전환 함수
	// 2) 위젯 애니메이션 관련 ----------------------------------------------------------------------------------
	void PlayResultAnimations();
	void OnRescueAnimationFinished();

////////// 사용자 정의형 함수 구간 ==============================================================================

};
