// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "Blueprint/UserWidget.h"
#include "KJH/KJH_Interface.h"
#include "KJH/KJH_UserWidget.h"

// ���� ������ ����� �� �ִ� ���� �̸��� ���� ����� ����
const static FName SESSION_NAME = TEXT("EXIT Session Game");

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // ������ ������ �� �����ϴ� ������.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // �÷��̸� ������ ���� �����ϴ� ������. �ʱ�ȭ�� �����ش�.
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(); // OnlineSubsystem ��������
	if (Subsystem) // ����, Subsystem�� ��ȿ�ϴٸ�,
	{
		SessionInterface = Subsystem->GetSessionInterface(); // ���� �������̽� ��������
		
		// ����, ���� �������̽��� ��ȿ�ϴٸ�,
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnCreateSessionComplete);
																// CreateSession�� �������� �޾Ƽ� OnCreateSessionComplete �Լ� ����

			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UKJH_GameInstance::OnFindSessionComplete);

			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			if (SessionSearch.IsValid())
			{

			SessionSearch->bIsLanQuery = true;
			
			SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
			// ToSharedRef -> TSharedPtr �� �׻� ��ȿ�ϰ� �ٲ��ִ� �޼���. TSharedptr �� Null�� ���� �ִµ�, 
			// FindSession�̶� �޼���� Null�̸� �����ϴϱ� ���ʿ� ��ȿ�� �༮�� �ְ� ��û�Ѵ�. �׷��� �츮�� �׻� ��ȿ�ϰ� ToSharedRef�� ��ȯ������Ѵ�.
			}

		}
	}
}

////////// ��������Ʈ ���ε� �Լ� ���� ���� ------------------------------------------------------------------------------
void UKJH_GameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	// ���� ������ ������ listen ������ �����ش�.
	GetWorld()->ServerTravel("/Game/MAPS/KJH/KJH_TestMap?listen");

	// ������ ���������� ������ �Ŀ��� UI�� �����ϴ� Teardown �Լ��� �����Ѵ�.
	if (ServerUI)
	{
		ServerUI->Teardown();
	}
}


// ���� �ı� �Ϸ� �� ȣ��Ǵ� �Լ�
void UKJH_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) // �ı��� �����ϸ�?
	{
		CreateSession(); // ������ ����������. (������ ������ ������ �װ��� �μ��� ���ο� ������ ����ٴ� ���̴�.)
	}
}


void UKJH_GameInstance::OnFindSessionComplete( bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));

	if (Success && SessionSearch.IsValid())
	{
		for (auto& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session Names : %s"), *SearchResult.GetSessionIdStr());
		}
	}
}

////////// ��������Ʈ ���ε� �Լ� ���� ���� ------------------------------------------------------------------------------


////////// ����� ������ �Լ� ���� ���� ----------------------------------------------------------------------------------

// ���� ���� �Լ�
void UKJH_GameInstance::Host()
{
	// ����, ���� �������̽��� ��ȿ�ϴٸ�,
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME); // ���� ���� ���� ���
		if (ExistingSession) // ����, ���� ������ �����ϸ�
		{
			SessionInterface->DestroySession(SESSION_NAME); // ������ ���� ������ �ı�
															// ����Ǹ� 'DestroySession'�� ��������Ʈ�� ������ �����Ѵ�. ��, �ٷ� ��������Ʈ�� ȣ��ȴ�.
		}

		else // ����, ���� ������ ���ٸ�,
		{
			CreateSession(); // ������ �����.
		}
	}
}

// ���� ���� �Լ�
void UKJH_GameInstance::Join(const FString& Address)
{
	GEngine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
													// �ش� �ɼ��� ���� ��θ� ����Ͽ� �̵��ϴ� ���� �ǹ�
													// ��, Ŭ���̾�Ʈ�� ��õ� ��Ȯ�� ���̳� ������ �̵���Ų��. 
													// �� �ɼ��� ����� ���� URL�� ������ �����Ǿ�� �Ѵ�.
	}
}

// ���� ���� �Լ�
void UKJH_GameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings; // CreateSession�� ���� ���Ƿ� ���Ǽ����� ������ش�.
		SessionSettings.bIsLANMatch = true ; // ���ÿ��� ã�� ���ΰ�?
		SessionSettings.NumPublicConnections = 5; // �÷��̾� ��
		SessionSettings.bShouldAdvertise = true; // �¶��ο��� ������ �� �� �ֵ�����. '������'

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings); // ������ �����Ѵ�. 
																		   // ����Ǹ� 'CreateSession'�� ��������Ʈ�� ������ �����Ѵ�. ��, �ٷ� ��������Ʈ�� ȣ��ȴ�.
																		   // ����(�÷��̾��ȣ, TEXT("�����̸�"), ���Ǽ���)
	}
}

// UI �Լ� (�׽�Ʈ��)
void UKJH_GameInstance::LoadMenu()
{

	// ServerUIFactory�� ���� ServerUI ���� ����
	ServerUI = CreateWidget<UKJH_MainServerUI>(this, ServerUIFactory);

	ServerUI -> Setup();

	ServerUI -> SetMyInterface(this);

}