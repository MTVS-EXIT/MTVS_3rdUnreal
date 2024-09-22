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

////////// ������ & �ʱ�ȭ �Լ� ���� ===========================================================================================
	
	UKJH_ServerWidget(const FObjectInitializer& ObjectInitialize); // ������ ����

	// ���� ���� �� �ڵ����� ȣ��Ǵ� �Լ�
	virtual void NativeConstruct() override;

	virtual bool Initialize(); // �ʱ�ȭ �Լ� ����


////////// TSubclass & class ���� ���� ==========================================================================================
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerRowFactory; // ServerRow(Text UI) ����
	class UKJH_ServerRow* ServerRow; // ServerRow(Text UI) ����

////////// UI ���ε� ���� =======================================================================================================

	// �޴� ü���� ���� //
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // �� �޴��� ��ȯ��ų �� �ִ� Menu Switcher

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowTransitionAnim;

	// 2-1) ���θ޴� UI ���� -----------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu; // ���� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_HostButton; // ������ ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_JoinButton; // �κ�޴��� ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_QuitButton; // �������� ��ư

	// 2-2) ���θ޴� UI �ִϸ��̼� ���� -------------------------------------------------
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowMainMenuAnim; // ���θ޴��� ��ȯ �� Show �ִϸ��̼� ����

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideMainMenuAnim; // ���θ޴� Hide �ִϸ��̼� ����

	// ----------------------------------------------------------------------------------
	
	// 3-1)�� ���� �޴� UI ���� // ------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu; // �� ���� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerHostName;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_ConfirmButton;

	// 3-2) �κ�޴� UI �ִϸ��̼� ���� -------------------------------------------------
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowHostMenuAnim; // �� ���� �޴��� ��ȯ �� Show �ִϸ��̼� ����

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideHostMenuAnim; // �� ���� �޴� Hide �ִϸ��̼� ����
	// -----------------------------------------------------------------------------------
	
	// 4-1) �κ�޴� UI ���� // ----------------------------------------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidget* LobbyMenu; // �κ� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_CancelButton; // �κ񿡼� ���θ޴��� �ٽ� ���ư��� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_JoinButton; // �κ񿡼� ���� �������� �����ϴ� ��ư

	// 4-2) �κ�޴� UI �ִϸ��̼� ���� -------------------------------------------------
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowLobbyMenuAnim; // ���θ޴��� ��ȯ �� Show �ִϸ��̼� ����

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideLobbyMenuAnim; // ���θ޴� Hide �ִϸ��̼� ����
	// -----------------------------------------------------------------------------------

	// �ӽ� //
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;


////////// ���� �� Ŭ���� ���� ���� -------------------------------------------------------------------------------------------

	TOptional <uint32> SelectedIndex; // ������ �ε����� ���� ����
	
	class UKJH_GameInstance* GameInstance; // GameInstance ���� ����

////////// ����� ������ �Լ� ���� --------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void CreateRoom(); // Interface���� Host �Լ��� ȣ���ϴ� �Լ�

	void SetServerList(TArray<FServerData> ServerNames); // ��ܿ� ����� FServerData ����ü�� �������� Session ����� �����ϴ� �Լ�

	void SelecetIndex(uint32 Index); // ������ �ε����� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void JoinRoom(); // ������ Session�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenHostMenu(); // �� ���� �޴��� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenLobbyMenu(); // �κ�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenMainMenu(); // ���θ޴��� ���ư��� �Լ�

	UFUNCTION(BlueprintCallable)
	void QuitPressed(); // ������ �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void UpdateChildren();

	// 2) ���� �ִϸ��̼� ���� �Լ� --------------------------------------------------------------------
    //UFUNCTION()
    //void OnShowMainMenuAnimFinished(); // �ִϸ��̼� ���� �� ȣ��� �Լ�
};
