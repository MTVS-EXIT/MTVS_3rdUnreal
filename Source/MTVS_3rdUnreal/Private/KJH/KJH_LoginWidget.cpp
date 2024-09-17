// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_LoginWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "KJH/KJH_GameInstance.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Delegates/Delegate.h"

#include "Engine/TimerHandle.h"
////////// ������ & �ʱ�ȭ �Լ� ���� ===================================================================
bool UKJH_LoginWidget::Initialize()
{
	Super::Initialize();


////////// ��ư Ŭ�� �̺�Ʈ ��������Ʈ ���� ====================================================================================================
	// �α��� �޴� ��ư ��������Ʈ ���ε�
	if (LoginMenu_RegisterButton)
	LoginMenu_RegisterButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OpenRegisterMenu); // CreateAccountButton ��ư ������ �� OpenRegisterMenu �Լ� ȣ��
	
	if (LoginMenu_LoginButton)
	LoginMenu_LoginButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnMyLogin); // LoginMenu_LoginButton ��ư ������ �� OnLoginToPlay �Լ� ȣ��

	if (LoginMenu_GuestLoginButton)
	LoginMenu_GuestLoginButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnMyGuestLogin); // GuestLoginButton ��ư ������ �� OnMyGuestLogin �Լ� ȣ��

	// ���� ���� �޴� ��ư ��������Ʈ ���ε�
	if (RegisterMenu_CreateButton)
	RegisterMenu_CreateButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnMyRegister); // CreateButton ��ư ������ �� OnRegisterMyInfo �Լ� ȣ��
	
	if (RegisterMenu_CancelButton)
	RegisterMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OpenLoginMenu); // CancelButton ��ư ������ �� OpenLoginMenu �Լ� ȣ��

	
	return true;
}

////////// ����� ������ �Լ� ���� - UI ��ȯ ���� ====================================================================================================
// �������� �޴� ��ȯ �Լ�
void UKJH_LoginWidget::OpenRegisterMenu()
{
	if (HideLoginMenuAnim)
	PlayAnimation(HideLoginMenuAnim);

	FTimerHandle TimerHandle_MenuSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch, [this]() 
	{
		if (MenuSwitcher)
		MenuSwitcher->SetActiveWidget(RegisterMenu);
		PlayAnimation(ShowRegisterMenuAnim);
	}, 1.0f, false);

	UE_LOG(LogTemp, Log, TEXT("OpenRegisterMenu called"));
}


// �α��� �޴� ��ȯ �Լ�
void UKJH_LoginWidget::OpenLoginMenu()
{

	if (HideRegisterMenuAnim)
	{
		PlayAnimation(HideRegisterMenuAnim);
	}

	FTimerHandle TimerHandle_MenuSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch, [this]()
	{
		if (MenuSwitcher)
		MenuSwitcher->SetActiveWidget(LoginMenu);
		PlayAnimation(ShowLoginMenuAnim);

	}, 1.0f, false);


	UE_LOG(LogTemp, Log, TEXT("OpenLoginMenu called"));

}

////////// ����� ������ �Լ� ���� - �������� ���� ====================================================================================================
// �������� ��û �Լ�
void UKJH_LoginWidget::OnMyRegister()
{
	// �Էµ� �α��� ������ ������.
	FString RegisterUserID = RegisterMenu_UserIDText->GetText().ToString();
	FString RegisterNickname = RegisterMenu_UserNicknameText->GetText().ToString();
	FString RegisterPassword = RegisterMenu_UserPasswordText->GetText().ToString();

	// �������� ������ ������ ������ URL ����
	FString URL = ""; // �鿣�� ���� URL

	// JSON ��ü(Object) ���� �� �Էµ� ���� �߰�
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject()); // ���ο� Json ��ü ����

	if (false == JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Create JsonObject"))
	}

	JsonObject->SetStringField("user_id", RegisterUserID); // UserID �ؽ�Ʈ�� �޾Ƽ� "user_id" Ű�� ����
	JsonObject->SetStringField("nickname", RegisterNickname); // Nickname �ؽ�Ʈ�� �޾Ƽ� "nickname" Ű�� ����
	JsonObject->SetStringField("password", RegisterPassword); // Password �ؽ�Ʈ�� �޾Ƽ� "password" Ű�� ����

	// JSON �����͸� ������ ������ �� �ִ� ���ڿ��� ��ȯ
	FString JsonPayload; // �����͸� ���� JsonPayload ��� �� ���ڿ� ����

	// JSON �����͸� ���ڿ��� �ٲٴ� ����(Writer)�� ����
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);
	
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer); // JSON ��ü�� ���ڿ��� ��ȯ�Ͽ� JsonPayload�� ����
	if (false == FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to serialize JSON object"));
		return;
	}

	// HTTP ��û�� POST ������� ������ ����
	SendRegisterRequest(URL, JsonPayload, "POST");
}

// �������� HTTP ��û�� ������ �Լ�
void UKJH_LoginWidget::SendRegisterRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType)
{
	// 1. HTTP ��û ��ü�� ����
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 2. ��û�� �Ϸ�Ǹ� ȣ��� �Լ� ���� (������ ó���ϴ� �Լ� ���ε�)
	Request->OnProcessRequestComplete().BindUObject(this, &UKJH_LoginWidget::OnRegisterResponseReceived);

	// 3. ��û�� URL ����
	FString RequestURL = ""; // ��û�� ó���� URL ����
	Request->SetURL(RequestURL);

	// 4. ��û Ÿ�� ���� (Post, Get ��)
	Request->SetVerb("POST");

	// 5. ��û ����� �����͸� � �������� �������� ���� (JSON ��)
	Request->SetHeader("Content-Type", "application/json");

	// 6. ������ ���� �����͸� ��� ��û�� ����
	Request->SetContentAsString(JsonPayload);

	// 7. ������ ��û�� ������ ����
	Request->ProcessRequest();
}

// �������� HTTP ��û ����� ó���ϴ� �Լ�
void UKJH_LoginWidget::OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 1. ��û ������ ���伺 ��ȿ�� ���� Ȯ��
	if (false == bWasSuccessful || false == Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Register request failed")); // ��û ���� �α�
		return;
	}

	// 2. �����κ��� ���� ������ ���ڿ��� ������
	FString ResponseContent = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("HTTP Response: %s"), *ResponseContent);  // ���� ������ �α׷� ���

	// 3. JSON ������ ó���ϱ� ���� JSON ��ü ����
	TSharedPtr<FJsonObject> JsonObject;

	// 4. ���� ������ Json �������� �б� ���� ���� ��ü ����
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// 5. JSON ������ �Ľ�(���� �ؼ�), ���������� �Ľ� ��, JsonObject�� ������ ����
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		// 6. ���� ���信�� "success" ��� �ʵ带 ã��, ���� ���θ� Ȯ��
		bool bSuccess = JsonObject->GetBoolField(TEXT("success"));

		if (bSuccess) // ���������� ���� ��, ���������� �Ϸ�Ǿ����ϴ�. UI ����
		{
			// UI �����ϴ� ���� (���� �߰�)
		}
		else // ���� �� �α� ���
		{
			UE_LOG(LogTemp, Warning, TEXT("Register Failed!"));
		}
	}
}

////////// ����� ������ �Լ� ���� - �α��� ���� =======================================================================================================
// �α��� ��û �Լ�
void UKJH_LoginWidget::OnMyLogin()
{
	// 1. �Էµ� ������ ������
	FString LoginUserID = LoginMenu_UserIDText->GetText().ToString();
	FString LoginPassword = LoginMenu_UserPasswordText->GetText().ToString();

	// 2. �α��� ������ ������ ������ URL ����
	FString URL = ""; // �鿣�� ���� URL

	//// 3. JSON ��ü ���� �� �Էµ� ���� �߰�
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if (false == JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Create JsonObject"));
	}

	JsonObject->SetStringField("user_id", LoginUserID);
	JsonObject->SetStringField("password", LoginPassword);

	// 4. JSON �����͸� ������ ������ �� �ִ� ���ڿ��� ��ȯ
	FString JsonPayload; // �����͸� ���� JsonPayload ��� �� ���ڿ� ����

	// 5. JSON �����͸� ���ڿ��� �ٲٴ� ����(Writer)�� ����
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);

	// 6. JSON ��ü�� ���ڿ��� ��ȯ�Ͽ� JsonPayload�� ����
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	if (false == FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to serialize JSON object"));
		return;
	}

	// 7. HTTP ��û�� POST ������� ������ ����
	SendLoginRequest(URL, JsonPayload, "POST");
}


// �α��� HTTP ��û�� ������ �Լ�
void UKJH_LoginWidget::SendLoginRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType)
{
	// 1. HTTP ��û ��ü�� ����
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 2. ��û�� �Ϸ�Ǹ� ȣ��� �Լ� ���� (������ ó���ϴ� �Լ� ���ε�)
	Request->OnProcessRequestComplete().BindUObject(this, &UKJH_LoginWidget::OnLoginResponseReceived);

	// 3. ��û�� URL ����
	FString RequestURL = ""; // ��û�� ó���� URL ����
	Request->SetURL(RequestURL);

	// 4. ��û Ÿ�� ���� (Post, Get ��)
	Request->SetVerb("POST");

	// 5. ��û ����� �����͸� � �������� �������� ���� (JSON ��)
	Request->SetHeader("Content-Type", "application/json");

	// 6. ������ ���� �����͸� ��� ��û�� ����
	Request->SetContentAsString(JsonPayload);

	// 7. ������ ��û�� ������ ����
	Request->ProcessRequest();
}

// �α��� HTTP ��û ����� ó���ϴ� �Լ�
void UKJH_LoginWidget::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 1. ��û ������ ���伺 ��ȿ�� ���� Ȯ��
	if (false == bWasSuccessful || false == Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Login request failed")); // ��û ���� �α�
		return;
	}

	// 2. �����κ��� ���� ������ ���ڿ��� ������
	FString ResponseContent = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("Login Response: %s"), *ResponseContent);  // ���� ������ �α׷� ���

	// 3. JSON ������ ó���ϱ� ���� JSON ��ü ����
	TSharedPtr<FJsonObject> JsonObject;

	// 4. ���� ������ Json �������� �б� ���� ���� ��ü ����
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// 5. JSON ������ �Ľ�(���� �ؼ�), ���������� �Ľ� ��, JsonObject�� ������ ����
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		// 6. ���� ���信�� "success" ��� �ʵ带 ã��, ���� ���θ� Ȯ��
		bool bSuccess = JsonObject->GetBoolField(TEXT("success"));

		if (bSuccess) // �α��ο� ���� ��, ���� �ν��Ͻ��� ������ ServerWidgetMap���� �̵�
		{
			UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance());
			if (GameInstance)
				GameInstance->LoadServerWidgetMap();
		}
		else // ���� �� �α� ���
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed!"));
		}

	}
}

////////// ����� ������ �Լ� ���� - �Խ�Ʈ �α��� ���� =======================================================================================================
void UKJH_LoginWidget::OnMyGuestLogin()
{
	UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
		GameInstance->LoadServerWidgetMap();
}
