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
	UKJH_ServerWidget(const FObjectInitializer& ObjectInitialize); // ������ ����

	virtual bool Initialize(); // BeginPlay?


////////// TSubclass & class ���� ���� -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerRowFactory; // ServerRow(Text UI) ����
	class UKJH_ServerRow* ServerRow; // ServerRow(Text UI) ����

////////// UI ���ε� ���� ------------------------------------------------------------------------------------------------------

	// �޴� ü���� ���� //
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // UI�� ü���� ��ų �� �ִ� Switcher

	// ���θ޴� UI ���� //
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu; // ���� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_HostButton; // ������ ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_JoinButton; // �κ�޴��� ���� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenu_QuitButton; // �������� ��ư


	// �κ�޴� UI ���� //
	UPROPERTY(meta = (BindWidget))
	class UWidget* LobbyMenu; // �κ� �޴� Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_CancelButton; // �κ񿡼� ���θ޴��� �ٽ� ���ư��� ��ư

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyMenu_JoinButton; // �κ񿡼� ���� �������� �����ϴ� ��ư

	// �ӽ� //
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;


////////// ���� ���� ���� -----------------------------------------------------------------------------------------------------

	TOptional <uint32> SelectedIndex; // ������ �ε����� ���� ����
	
	class UKJH_GameInstance* GameInstance; // GameInstance ���� ����

////////// ����� ������ �Լ� ���� --------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void HostServer(); // Interface���� Host �Լ��� ȣ���ϴ� �Լ�

	void SetServerList(TArray<FServerData> ServerNames); // ��ܿ� ����� FServerData ����ü�� �������� Session ����� �����ϴ� �Լ�

	void SelecetIndex(uint32 Index); // ������ �ε����� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void JoinServer(); // ������ Session�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenLobbyMenu(); // �κ�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OpenMainMenu(); // ���θ޴��� ���ư��� �Լ�

	UFUNCTION(BlueprintCallable)
	void QuitPressed(); // ������ �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void UpdateChildren(); // 
};
