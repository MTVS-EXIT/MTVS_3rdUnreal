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

////////// 클래스 참조 구간 ------------------------------------------------------------

	class IKJH_Interface* MyInterface;

////////// 바인딩 구간 -----------------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;	

////////// 사용자 정의형 함수 구간 -----------------------------------------------------
	UFUNCTION()
	void HostServer();

};
