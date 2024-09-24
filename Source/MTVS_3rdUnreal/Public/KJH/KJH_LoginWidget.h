// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_WidgetSystem.h"
#include "HttpFwd.h"
#include "KJH_LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_LoginWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()
	

public :

////////// 생성자 & 초기화 함수 구간 ===================================================================
	virtual bool Initialize(); // 초기화 함수

////////// UI 바인딩 구간 ==============================================================================
	// 1) 메뉴 체인지 관련 ---------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 메뉴로 전환시킬 수 있는 Menu Switcher

	// 2) 로그인 메뉴 관련 ---------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowLoginMenuAnim; // 로그인 메뉴 Show 애니메이션

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideLoginMenuAnim; // 로그인 메뉴 Hide 애니메이션

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* LoginMenu; // 로그인 메뉴 위젯

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_RegisterButton; // 계정 생성 메뉴로 전환하는 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_LoginButton; // 로그인 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* LoginMenu_UserIDText; // 로그인 메뉴의 아이디 입력 필드

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginMenu_UserPasswordText; // 로그인 메뉴의 패스워드 입력 필드

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UImage* LoginMenu_InvalidAccessImage; // 로그인 실패 이미지

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_InvalidAccessFinishButton; // 로그인 실패 시 완료 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginMenu_GuestLoginButton; // Guest 로그인 버튼 -> 바로 서버메뉴로 넘어갈 수 있음.

	// 3) 계정생성 메뉴 관련 --------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowRegisterMenuAnim; // 계정생성 메뉴 Show 애니메이션

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideRegisterMenuAnim; // 계정생성 메뉴 Hide 애니메이션

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* RegisterMenu; // 계정생성 메뉴 위젯

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_CreateButton; // 계정 생성 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_CancelButton; // 로그인 메뉴로 전환 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_UserIDText; // 계정생성 메뉴의 아이디 입력 필드

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_UserNicknameText; // 계정생성 메뉴의 닉네임 입력 필드

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_UserPasswordText; // 계정생성 메뉴의 패스워드 입력 필드

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UImage* RegisterMenu_CreateSuccessImage; // 계정생성 성공 이미지

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_SuccessFinishButton; // 계정생성 성공 시 완료 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UImage* RegisterMenu_AlreadyExistImage; // 계정생성 실패 이미지

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_FailureFinishButton; // 계정생성 실패 시 완료 버튼

////////// 사용자 정의형 함수 구간 - UI 전환 관련 ====================================================================================================

	UFUNCTION(BlueprintCallable)
	void OpenRegisterMenu(); // 계정생성 메뉴로 전환 함수

	UFUNCTION(BlueprintCallable)
	void OpenLoginMenu(); // 계정생성 메뉴로 전환 함수
	
////////// 사용자 정의형 함수 구간 - 계정생성 관련 ====================================================================================================
	UFUNCTION(BlueprintCallable)
	void OnMyRegister(); // 계정생성 요청 함수

	void SendRegisterRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType); // 계정생성 HTTP 요청을 보내는 함수
	void OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); // 계정생성 HTTP 요청 결과를 처리하는 함수
	
	UFUNCTION()
	void ShowRegisterSuccessUI(); // 계정생성 성공 시 나타나는 UI 함수
	UFUNCTION()
    void OnRegisterSuccessFinishClicked(); // 계정생성 성공 시 나타나는 확인 버튼 이벤트 처리 함수
	UFUNCTION()
	void ShowRegisterFailureUI(); // 계정생성 실패 시 나타나는 UI 함수
    UFUNCTION()
    void OnRegisterFailureFinishClicked(); // 계정생성 실패 시 나타나는 확인 버튼 이벤트 처리 함수
	
////////// 사용자 정의형 함수 구간 - 로그인 관련 =======================================================================================================
	UFUNCTION(BlueprintCallable)
	void OnMyLogin(); // 로그인 요청 함수

	void SendLoginRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType); // 로그인 HTTP 요청을 보내는 함수
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); // 로그인 HTTP 요청 결과를 처리하는 함수

    UFUNCTION()
    void ShowLoginFailureUI(); // 로그인 실패 시 나타나는 UI 함수
    
    UFUNCTION()
    void OnLoginFailureFinishClicked();	// 로그인 실패 시 나타나는 확인 버튼 이벤트 처리 함수
////////// 사용자 정의형 함수 구간 - 게스트 로그인 관련 =======================================================================================================
	UFUNCTION(BlueprintCallable)
	void OnMyGuestLogin();

};
