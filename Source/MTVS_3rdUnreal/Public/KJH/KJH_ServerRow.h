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

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ServerName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RowButton;

	UPROPERTY()
	class UKJH_ServerWidget* Parent;
	uint32 Index;

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false; // 서버를 선택했는지, 아닌지 선언

	void Setup(class UKJH_ServerWidget* Parent, uint32 Index);

	UFUNCTION()
	void OnClicked();
};
