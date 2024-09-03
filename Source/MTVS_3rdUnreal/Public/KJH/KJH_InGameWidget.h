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

////////// 바인딩 구간 -----------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_CancelButton; // 인게임 UI -> 게임으로 복귀하는 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* InGameMenu_QuitButton; // 인게임 UI -> 게임을 종료하는 버튼

	////////// 사용자 정의형 함수 구간 -----------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void CancelPressed();

	UFUNCTION(BlueprintCallable)
	void QuitPressed();
};
