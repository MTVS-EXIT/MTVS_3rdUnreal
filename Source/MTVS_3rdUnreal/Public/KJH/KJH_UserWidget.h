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
	class UButton* MainMenu_HostButton; // 서버를 여는 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_JoinButton; // 로비메뉴로 가는 버튼


	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_CancelButton; // 로비에서 메인메뉴로 다시 돌아가는 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_JoinButton; // 로비에서 실제 세션으로 접속하는 버튼

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI를 체인지 시킬 수 있는 Switcher

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu; // 메인 UI

	UPROPERTY(meta = (BindWidget))
	class UWidget* LobbyMenu; // 로비 UI

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressField;


////////// 사용자 정의형 함수 구간 -----------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void HostServer(); // Interface에서 Host 함수를 호출하는 함수


	void Setup(); // ServerUI를 Setup 하는 함수

	void Teardown(); // ServerUI를 다 사용했을 경우, 제거하는 함수

	UFUNCTION(BlueprintCallable)
	void OpenLobbyMenu(); // 로비로 접속하는 함수

	UFUNCTION(BlueprintCallable)
	void OpenMainMenu(); // 메인레벨로 돌아가는 함수

	UFUNCTION(BlueprintCallable)
	void JoinServer(); // Session에 접속하는 함수
};
