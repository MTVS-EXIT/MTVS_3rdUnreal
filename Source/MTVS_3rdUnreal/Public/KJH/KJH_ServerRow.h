// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KJH_ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_ServerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:

////////// UI 바인딩 구간 ------------------------------------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RowButton; // 모든 하위 UI 개체를 그룹화한 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ServerName; // 서버 명

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* HostUser; // 개설자 명

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ConnectedPlayer; // 접속자 수


	UPROPERTY()
	class UKJH_ServerWidget* Parent;
	uint32 Index;

////////// 변수 및 클래스 선언 구간 -------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly)
	bool Selected = false; // 서버를 선택했는지, 아닌지 선언

////////// 사용자 정의형 함수 선언 구간 ---------------------------------------------------------------------------------------
	void Setup(class UKJH_ServerWidget* Parent, uint32 Index);

	UFUNCTION()
	void OnClicked();
};
