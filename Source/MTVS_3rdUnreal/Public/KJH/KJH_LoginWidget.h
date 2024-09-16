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

////////// ������ & �ʱ�ȭ �Լ� ���� ===================================================================
	virtual bool Initialize(); // �ʱ�ȭ �Լ�

////////// UI ���ε� ���� ==============================================================================
	// 1) �޴� ü���� ���� ---------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // �� �޴��� ��ȯ��ų �� �ִ� Menu Switcher

	// 2) �α��� �޴� ���� ---------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AppearLoginMenuAnim;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* DisappearLoginMenuAnim;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* LoginMenu; // �α��� �޴� ����

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_RegisterButton; // ���� ���� �޴��� ��ȯ�ϴ� ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_LoginButton; // �α��� ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* LoginMenu_UserIDText; // �α��� �޴��� ���̵� �Է� �ʵ�

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginMenu_UserPasswordText; // �α��� �޴��� �н����� �Է� �ʵ�

	// 3) �������� �޴� ���� --------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AppearRegisterMenuAnim;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* DisappearRegisterMenuAnim;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* RegisterMenu; // �������� �޴� ����

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_CreateButton; // ���� ���� ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_CancelButton; // �α��� �޴��� ��ȯ ��ư

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_UserIDText; // �������� �޴��� ���̵� �Է� �ʵ�

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_UserNicknameText; // �������� �޴��� �г��� �Է� �ʵ�

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_UserPasswordText; // �������� �޴��� �н����� �Է� �ʵ�

////////// ����� ������ �Լ� ���� - UI ��ȯ ���� ====================================================================================================

	UFUNCTION(BlueprintCallable)
	void OpenRegisterMenu(); // �������� �޴��� ��ȯ �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenLoginMenu(); // �������� �޴��� ��ȯ �Լ�
	
////////// ����� ������ �Լ� ���� - �������� ���� ====================================================================================================
	UFUNCTION(BlueprintCallable)
	void OnMyRegister(); // �������� ��û �Լ�

	void SendRegisterRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType); // �������� HTTP ��û�� ������ �Լ�
	void OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); // �������� HTTP ��û ����� ó���ϴ� �Լ�

////////// ����� ������ �Լ� ���� - �α��� ���� =======================================================================================================
	UFUNCTION(BlueprintCallable)
	void OnMyLogin(); // �α��� ��û �Լ�

	void SendLoginRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType); // �α��� HTTP ��û�� ������ �Լ�
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); // �α��� HTTP ��û ����� ó���ϴ� �Լ�
};
