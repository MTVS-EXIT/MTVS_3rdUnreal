// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KJH/KJH_WidgetSystem.h"
#include "KJH_Interface.h"
#include "KJH_ServerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_ServerWidget : public UKJH_WidgetSystem
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	virtual bool Initialize();


////////// ���ε� ���� -----------------------------------------------------------------
	// ���θ޴� UI ����
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu; // ���� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_HostButton; // ������ ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_JoinButton; // �κ�޴��� ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_QuitButton; // �������� ��ư


	// �κ�޴� UI ����
	UPROPERTY(meta = (BindWidget))
	class UWidget* LobbyMenu; // �κ� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_CancelButton; // �κ񿡼� ���θ޴��� �ٽ� ���ư��� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_JoinButton; // �κ񿡼� ���� �������� �����ϴ� ��ư


	// �޴� ü���� ����
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI�� ü���� ��ų �� �ִ� Switcher

	// �ӽ�
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressField;


////////// ����� ������ �Լ� ���� -----------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void HostServer(); // Interface���� Host �Լ��� ȣ���ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void JoinServer(); // Session�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenLobbyMenu(); // �κ�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenMainMenu(); // ���θ޴��� ���ư��� �Լ�

	UFUNCTION(BlueprintCallable)
	void QuitPressed(); // ������ �����ϴ� �Լ�
};
