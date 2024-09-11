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

////////// UI ���ε� ���� ------------------------------------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RowButton; // ��� ���� UI ��ü�� �׷�ȭ�� ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ServerName; // ���� ��

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* HostUser; // ������ ��

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ConnectedPlayer; // ������ ��


	UPROPERTY()
	class UKJH_ServerWidget* Parent;
	uint32 Index;

////////// ���� �� Ŭ���� ���� ���� -------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly)
	bool Selected = false; // ������ �����ߴ���, �ƴ��� ����

////////// ����� ������ �Լ� ���� ���� ---------------------------------------------------------------------------------------
	void Setup(class UKJH_ServerWidget* Parent, uint32 Index);

	UFUNCTION()
	void OnClicked();
};
