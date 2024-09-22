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

////////// 초기화 함수 구간 ===================================================================
	virtual bool Initialize(); // UserWidget 초기화 함수

	virtual void Setup() override;
////////// 바인딩 구간 ========================================================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InGameMenuAnim; // 인게임 UI 호출 시 등장하는 애니메이션 참조

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_CancelButton; // 인게임 UI -> 게임으로 복귀하는 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_QuitButton; // 인게임 UI -> 게임을 종료하여 메인메뉴로 이동하는 버튼

////////// 사용자 정의형 함수 선언 구간 ========================================================

	UFUNCTION(BlueprintCallable)
	void CancelPressed();

	UFUNCTION(BlueprintCallable)
	void QuitPressed();
};
