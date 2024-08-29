// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KJH_Interface.h"
#include "KJH_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	virtual bool Initialize();

	void SetMyInterface(IKJH_Interface* Interface);

////////// Ŭ���� ���� ���� ------------------------------------------------------------

	class IKJH_Interface* MyInterface;

////////// ���ε� ���� -----------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;	

////////// ����� ������ �Լ� ���� -----------------------------------------------------
	UFUNCTION()
	void HostServer();

};
