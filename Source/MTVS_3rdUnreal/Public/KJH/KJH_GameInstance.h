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


////////// ����� ������ �Լ� ���� --------------------------------------------------------------------------------
	UFUNCTION(Exec) // Exec: �ܼ�â�� �Է��� �� �ֵ��� �����.
	void Host(); // ���� ���� �Լ�

	UFUNCTION(Exec)
	void Join(const FString& Address); // ���� ���� �Լ�

	UFUNCTION()
	void CreateSession(); // ������ ����� �Լ�

	UFUNCTION(BlueprintCallable)
	void LoadMenu(); // ���� ���� UI�� �ҷ����� �Լ�


////////// TSubclass & class ���� ���� -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerUIFactory; // ServerUI ����

	class UKJH_ServerWidget* ServerUI; // ServerUI ����

////////// ���� ���� & �ν��Ͻ� ���� -----------------------------------------------------------------------------------------

	IOnlineSessionPtr SessionInterface; // ���� �������̽��� �����μ��� ����
	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // �¶��� ���� �˻��� �� �� �ִ� Ŭ���� �ν��Ͻ� ����
	

};
