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
	class UButton* MainMenu_HostButton; // ������ ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_JoinButton; // �κ�޴��� ���� ��ư


	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_CancelButton; // �κ񿡼� ���θ޴��� �ٽ� ���ư��� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_JoinButton; // �κ񿡼� ���� �������� �����ϴ� ��ư

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI�� ü���� ��ų �� �ִ� Switcher

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu; // ���� UI

	UPROPERTY(meta = (BindWidget))
	class UWidget* LobbyMenu; // �κ� UI

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressField;


////////// ����� ������ �Լ� ���� -----------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void HostServer(); // Interface���� Host �Լ��� ȣ���ϴ� �Լ�


	void Setup(); // ServerUI�� Setup �ϴ� �Լ�

	void Teardown(); // ServerUI�� �� ������� ���, �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenLobbyMenu(); // �κ�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenMainMenu(); // ���η����� ���ư��� �Լ�

	UFUNCTION(BlueprintCallable)
	void JoinServer(); // Session�� �����ϴ� �Լ�
};
