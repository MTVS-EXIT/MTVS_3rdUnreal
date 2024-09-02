// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KJH/KJH_WidgetSystem.h"
#include "KJH_Interface.h"
#include "KJH_ServerWidget.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_ServerWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()

public:
	UKJH_ServerWidget(const FObjectInitializer& ObjectInitialize); // 생성자 선언

	virtual bool Initialize(); // BeginPlay?


////////// TSubclass & class 참조 구간 -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerRowFactory; // ServerRow(Text UI) 공장
	class UKJH_ServerRow* ServerRow; // ServerRow(Text UI) 선언

////////// UI 바인딩 구간 ------------------------------------------------------------------------------------------------------
	// 메인메뉴 UI 관련 //
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu; // 메인 메뉴 Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_HostButton; // 서버를 여는 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_JoinButton; // 로비메뉴로 가는 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_QuitButton; // 게임종료 버튼


	// 로비메뉴 UI 관련 //
	UPROPERTY(meta = (BindWidget))
	class UWidget* LobbyMenu; // 로비 메뉴 Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_CancelButton; // 로비에서 메인메뉴로 다시 돌아가는 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_JoinButton; // 로비에서 실제 세션으로 접속하는 버튼


	// 메뉴 체인지 관련 //
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI를 체인지 시킬 수 있는 Switcher


	// 임시 //
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

////////// 변수 참조 구간 -----------------------------------------------------------------------------------------------------

	TOptional <uint32> SelectedIndex; // 서버의 인덱스를 참조

////////// 사용자 정의형 함수 구간 --------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void HostServer(); // Interface에서 Host 함수를 호출하는 함수

	void SetServerList(TArray<FString> ServerNames); // 생성된 Session 목록을 설정하는 함수

	void SelecetIndex(uint32 Index); // 서버의 인덱스를 선택하는 함수

	UFUNCTION(BlueprintCallable)
	void JoinServer(); // 생성된 Session에 접속하는 함수

	UFUNCTION(BlueprintCallable)
	void OpenLobbyMenu(); // 로비로 접속하는 함수

	UFUNCTION(BlueprintCallable)
	void OpenMainMenu(); // 메인메뉴로 돌아가는 함수

	UFUNCTION(BlueprintCallable)
	void QuitPressed(); // 게임을 종료하는 함수

	UFUNCTION(BlueprintCallable)
	void UpdateChildren(); // 

};
