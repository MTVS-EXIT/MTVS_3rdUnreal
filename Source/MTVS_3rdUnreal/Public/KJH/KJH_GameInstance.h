// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "KJH_Interface.h"
#include "UObject/Interface.h"
#include "KJH_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_GameInstance : public UGameInstance, public IKJH_Interface
{
	GENERATED_BODY()
	
public:

// GameInstance�� ������ ���� �� ������ �Լ� ���� (�ʱ�ȭ�� ����)
	UKJH_GameInstance(const FObjectInitializer& ObjectInitializer);

// GameInstance�� BeginPlay �� ������ �Լ� ���� (�ʱ�ȭ�� ����)
	virtual void Init() override;



////////// ��������Ʈ ���ε� �Լ� ���� ----------------------------------------------------------------------------
	
	void OnCreateSessionComplete(FName SessionName, bool Success); // ���� ���� �Ϸ� �� ȣ��� �Լ�
	void OnDestroySessionComplete(FName SessionName, bool Success); // ���� �ı� �Ϸ� �� ȣ��� �Լ�
	void OnFindSessionComplete(bool Success); // ���� ã�� �Ϸ� �� ȣ��� �Լ�. Success ���ڸ� ������ �ȴ�.
											  // �߰ߵ� ���� ����� SeesionSearch TSharedRef �����Ϳ� �ֱ� �����̴�.

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

////////// ����� ������ �Լ� ���� --------------------------------------------------------------------------------
	UFUNCTION(Exec) // Exec: �ܼ�â�� �Է��� �� �ֵ��� �����.
	void Host(); // ���� ���� �Լ�

	UFUNCTION(Exec)
	void Join(uint32 Index); // ���� ���� �Լ�

	UFUNCTION()
	void CreateSession(); // ������ ����� �Լ�

	UFUNCTION(BlueprintCallable)
	void LoadServerWidget(); // ����ȭ�� UI�� �ҷ����� �Լ�

	UFUNCTION()
	void RefreshServerList();

	UFUNCTION(BlueprintCallable, Category = "Load Widget")
	void LoadInGameWidget(); // �ΰ��� UI�� �ҷ����� �Լ�

	UFUNCTION(BlueprintCallable, Category = "Load Widget")
	void LoadServerWidgetMap(); // �ΰ��� UI�� �ִ� ������ �ε��ϴ� �Լ� (UI�� ������ �پ��ֱ� ����)

	// ĳ���� ���� ���� �Լ� //
	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void OnCharacterSelected(bool bIsSelectedPersonFromUI); // �÷��̾ ������ ĳ���Ϳ� ���� ��Ʈ�ѷ��� ���õǴ� �Լ�


////////// ����� ������ ���� �Լ� ���� --------------------------------------------------------------------------------

////////// TSubclass & class ���� ���� -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ServerWidgetFactory; // ServerWidget(UI) ����
	class UKJH_ServerWidget* ServerWidget; // ServerWidget(UI) ���� ����

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> InGameWidgetFactory; // InGameWidget(UI) ����
	class UKJH_WidgetSystem* InGameWidget; // InGameWidget(UI) ���� ����

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) ����
	class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) ���� ����


	class AKJH_PlayerState* PlayerState; // PlayerState ���� ����

	class AKJH_GameModeBase* GameMode; // GameMode ����

////////// ���� ���� & �ν��Ͻ� ���� -----------------------------------------------------------------------------------------

	IOnlineSessionPtr SessionInterface; // ���� �������̽��� �����μ��� ����
	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // �¶��� ���� �˻��� �� �� �ִ� Ŭ���� �ν��Ͻ� ����

	// ĳ���� ���� ���� ����
	bool bIsPersonSelected = false; // UI �󿡼� ����� ���õǾ����� üũ
	bool bIsDroneSelected = false;  // UI �󿡼� ����� ���õǾ����� üũ

////////// Ÿ�̸��ڵ� ���� ���� -----------------------------------------------------------------------------------------
	FTimerHandle TimerHandle_CharacterSelect;
};
