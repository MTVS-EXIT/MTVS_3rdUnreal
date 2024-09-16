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

////////// ������ & �ʱ�ȭ �Լ� ���� ===========================================================================================

	UKJH_GameInstance(const FObjectInitializer& ObjectInitializer); // GameInstance�� ������ ���� �� ������ �Լ� ���� (�ʱ�ȭ�� ����)

	virtual void Init() override; // GameInstance�� BeginPlay �� ������ �Լ� ���� (�ʱ�ȭ�� ����)

////////// ��������Ʈ ���ε� �Լ� ���� =============================================================================
	
	void OnCreateSessionComplete(FName SessionName, bool Success); // ���� ���� �Ϸ� �� ȣ��� �Լ�
	void OnDestroySessionComplete(FName SessionName, bool Success); // ���� �ı� �Ϸ� �� ȣ��� �Լ�
	void OnFindSessionComplete(bool Success); // ���� ã�� �Ϸ� �� ȣ��� �Լ�. Success ���ڸ� ������ �ȴ�.
											  // �߰ߵ� ���� ����� SeesionSearch TSharedRef �����Ϳ� �ֱ� �����̴�.

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	
////////// ����� ������ �Լ� ���� =================================================================================
	
	// 1) ���� ���� �Լ� --------------------------------
	UFUNCTION(Exec) // Exec: �ܼ�â�� �Է��� �� �ֵ��� �����.
	void Host(FString ServerName) override; // ���� ���� �Լ�

	UFUNCTION(Exec)
	void Join(uint32 Index); // ���� ���� �Լ�

	UFUNCTION()
	void CreateSession(); // ������ ����� �Լ�

	UFUNCTION()
	void RefreshServerList(); // ��������� ã�� �Լ�

	// 2) UI ���� ���� �Լ� ----------------------------------
	UFUNCTION(BlueprintCallable, Category = "Create Widget")
	void CreateLoginWidget(); // �α��� UI�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable, Category = "Create Widget")
	void CreateServerWidget(); // ����ȭ�� UI�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable, Category = "Create Widget")
	void CreateInGameWidget(); // �ΰ��� UI�� �����ϴ� �Լ�




	UFUNCTION(BlueprintCallable, Category = "Load Widget Map")
	void LoadServerWidgetMap(); // ServerWidget UI�� �ִ� ������ �ε��Ű�� �Լ� (UI�� ������ Attach �س�����.)


	// ĳ���� ���� ���� �Լ� ----------------------------------
	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void OnCharacterSelected(APlayerController* PlayerController, bool bIsSelectedPersonFromUI); // �÷��̾ ������ ĳ���Ϳ� ���� ��Ʈ�ѷ��� ���õǴ� �Լ�

    UFUNCTION(Server, Reliable, WithValidation) // ServerRPC �� Ŭ���̾�Ʈ�� �������� ��û�ϴ� ���̹Ƿ�, Validate�� Implementation ���� ������ ����
    void ServerNotifyCharacterSelected(APlayerController* PlayerController, bool bIsSelectedPerson); // �������� Ŭ���̾�Ʈ�� ĳ���� ������ ó���ϴ� RPC �Լ� ����
	bool ServerNotifyCharacterSelected_Validate(APlayerController* PlayerController, bool bIsSelectedPerson); // ���� RPC �Լ��� ��ȿ�� �˻� (�� ������ ������ �Ǵ���, �ƴ���)
	void ServerNotifyCharacterSelected_Implementation(APlayerController* PlayerController, bool bIsSelectedPerson); // ���� RPC �Լ��� ������ (��û�� ���εǸ�, ������ ���⼭ ����)

////////// TSubclass & class ���� ���� -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_LoginWidget> LoginWidgetFactory; // ServerWidget(UI) ����
	class UKJH_LoginWidget* LoginWidget; // ServerWidget(UI) ���� ����
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_ServerWidget> ServerWidgetFactory; // ServerWidget(UI) ����
	class UKJH_ServerWidget* ServerWidget; // ServerWidget(UI) ���� ����

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_InGameWidget> InGameWidgetFactory; // InGameWidget(UI) ����
	class UKJH_InGameWidget* InGameWidget; // InGameWidget(UI) ���� ����

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) ����
	class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) ���� ����

////////// ���� ���� & �ν��Ͻ� ���� ���� -------------------------------------------------------------------------------

	IOnlineSessionPtr SessionInterface; // ���� �������̽��� �����μ��� ����
	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // �¶��� ���� �˻��� �� �� �ִ� Ŭ���� �ν��Ͻ� ����

	// ĳ���� ���� ���� ����
	bool bIsPersonSelected = false; // UI �󿡼� ����� ���õǾ����� üũ
	bool bIsDroneSelected = false;  // UI �󿡼� ����� ���õǾ����� üũ
	FString DesiredServerName; // Host �� ���� �̸��� �����Ͽ� �����ϱ� ���� ����

////////// Ÿ�̸��ڵ� ���� ���� -----------------------------------------------------------------------------------------
};
