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
#include "Components/Image.h"
////////// 생성자 & 초기화 함수 구간 ===================================================================
bool UKJH_LoginWidget::Initialize()
{
	Super::Initialize();

	if (RegisterMenu_CreateSuccessImage)
		RegisterMenu_CreateSuccessImage->SetVisibility(ESlateVisibility::Hidden);

	if (RegisterMenu_SuccessFinishButton)
	{
		RegisterMenu_SuccessFinishButton->SetVisibility(ESlateVisibility::Hidden);
		RegisterMenu_SuccessFinishButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnRegisterSuccessFinishClicked);
	}

	if (RegisterMenu_AlreadyExistImage)
		RegisterMenu_AlreadyExistImage->SetVisibility(ESlateVisibility::Hidden);

	if (RegisterMenu_FailureFinishButton)
	{
		RegisterMenu_FailureFinishButton->SetVisibility(ESlateVisibility::Hidden);
		RegisterMenu_FailureFinishButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnRegisterFailureFinishClicked);
	}

	if (LoginMenu_InvalidAccessImage)
		LoginMenu_InvalidAccessImage->SetVisibility(ESlateVisibility::Hidden);

	if (LoginMenu_InvalidAccessFinishButton)
	{
		LoginMenu_InvalidAccessFinishButton->SetVisibility(ESlateVisibility::Hidden);
		LoginMenu_InvalidAccessFinishButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnLoginFailureFinishClicked);
	}

////////// 버튼 클릭 이벤트 델리게이트 구간 ====================================================================================================
	// 로그인 메뉴 버튼 델리게이트 바인딩
	if (LoginMenu_RegisterButton)
	LoginMenu_RegisterButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OpenRegisterMenu); // CreateAccountButton 버튼 눌렀을 때 OpenRegisterMenu 함수 호출
	
	if (LoginMenu_LoginButton)
	LoginMenu_LoginButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnMyLogin); // LoginMenu_LoginButton 버튼 눌렀을 때 OnLoginToPlay 함수 호출

	if (LoginMenu_GuestLoginButton)
	LoginMenu_GuestLoginButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnMyGuestLogin); // GuestLoginButton 버튼 눌렀을 때 OnMyGuestLogin 함수 호출


	// 계정 생성 메뉴 버튼 델리게이트 바인딩
	if (RegisterMenu_CreateButton)
	RegisterMenu_CreateButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OnMyRegister); // CreateButton 버튼 눌렀을 때 OnRegisterMyInfo 함수 호출
	
	if (RegisterMenu_CancelButton)
	RegisterMenu_CancelButton->OnClicked.AddDynamic(this, &UKJH_LoginWidget::OpenLoginMenu); // CancelButton 버튼 눌렀을 때 OpenLoginMenu 함수 호출

	
	return true;
}

////////// 사용자 정의형 함수 구간 - UI 전환 관련 ====================================================================================================
// 계정생성 메뉴 전환 함수
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


// 로그인 메뉴 전환 함수
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

////////// 사용자 정의형 함수 구간 - 계정생성 관련 ====================================================================================================
// 계정생성 요청 함수
void UKJH_LoginWidget::OnMyRegister()
{
	// 입력된 로그인 정보를 가져옴.
	FString RegisterUserID = RegisterMenu_UserIDText->GetText().ToString();
	FString RegisterNickname = RegisterMenu_UserNicknameText->GetText().ToString();
	FString RegisterPassword = RegisterMenu_UserPasswordText->GetText().ToString();

	// 계정생성 정보를 서버로 전송할 URL 설정
	FString URL = "http://125.132.216.190:7757/api/auth/signup"; // 백엔드 서버 URL

	// JSON 객체(Object) 생성 후 입력된 정보 추가
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject()); // 새로운 Json 객체 생성

	if (false == JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Create JsonObject"))
	}

	JsonObject->SetStringField("loginId", RegisterUserID); // UserID 텍스트를 받아서 "user_id" 키에 저장
	JsonObject->SetStringField("nickname", RegisterNickname); // Nickname 텍스트를 받아서 "nickname" 키에 저장
	JsonObject->SetStringField("password", RegisterPassword); // Password 텍스트를 받아서 "password" 키에 저장

	// JSON 데이터를 서버가 이해할 수 있는 문자열로 변환
	FString JsonPayload; // 데이터를 담을 JsonPayload 라는 빈 문자열 생성

	// JSON 데이터를 문자열로 바꾸는 도구(Writer)를 생성
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);
	
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer); // JSON 객체를 문자열로 변환하여 JsonPayload에 저장
	if (false == FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to serialize JSON object"));
		return;
	}

	// HTTP 요청을 POST 방식으로 서버로 전송
	SendRegisterRequest(URL, JsonPayload, "POST");
}

// 계정생성 HTTP 요청을 보내는 함수
void UKJH_LoginWidget::SendRegisterRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType)
{
	// 1. HTTP 요청 객체를 생성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 2. 요청이 완료되면 호출될 함수 지정 (응답을 처리하는 함수 바인딩)
	Request->OnProcessRequestComplete().BindUObject(this, &UKJH_LoginWidget::OnRegisterResponseReceived);

	// 3. 요청할 URL 설정
	FString RequestURL = "http://125.132.216.190:7757/api/auth/signup"; // 요청을 처리할 URL 설정
	Request->SetURL(RequestURL);

	// 4. 요청 타입 설정 (Post, Get 등)
	Request->SetVerb("POST");

	// 5. 요청 헤더에 데이터를 어떤 형식으로 전송할지 설정 (JSON 등)
	Request->SetHeader("Content-Type", "application/json");

	// 6. 서버에 보낼 데이터를 담아 요청에 설정
	Request->SetContentAsString(JsonPayload);

	// 7. 설정한 요청을 서버로 전송
	Request->ProcessRequest();
}

// 계정생성 HTTP 요청 결과를 처리하는 함수
void UKJH_LoginWidget::OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 1. 요청 성공과 응답성 유효에 대해 확인
	if (false == bWasSuccessful || false == Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Register request failed")); // 요청 실패 로그
		ShowRegisterFailureUI();
		return;
	}

	// 2. 서버로부터 받은 응답을 문자열로 가져옴
	FString ResponseContent = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("HTTP Response: %s"), *ResponseContent);  // 받은 응답을 로그로 출력

	// 3. JSON 응답을 처리하기 위해 JSON 객체 생성
	TSharedPtr<FJsonObject> JsonObject;

	// 4. 응답 내용을 Json 형식으로 읽기 위한 리더 객체 생성
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// 5. JSON 응답을 파싱(구문 해석), 성공적으로 파싱 시, JsonObject에 데이터 삽입

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		// 6. 서버 응답에서 "success" 라는 필드를 찾아, 성공 여부를 확인
		if (JsonObject->HasField(TEXT("success")))
		{
			bool bSuccess = JsonObject->GetBoolField(TEXT("success"));
			if (bSuccess)
			{
				ShowRegisterSuccessUI(); // 계정생성에 성공 시, 계정생성이 완료되었습니다. UI 송출
			}
			else
			{
				FString errorMessage;
				if (JsonObject->TryGetStringField(TEXT("message"), errorMessage))
				{
					UE_LOG(LogTemp, Warning, TEXT("Register Failed: %s"), *errorMessage);
				}
				ShowRegisterFailureUI();
			}
		}
		else if (JsonObject->HasField(TEXT("error")))
		{
			// 에러 필드가 있는 경우
			FString errorMessage = JsonObject->GetStringField(TEXT("error"));
			UE_LOG(LogTemp, Warning, TEXT("Register Failed: %s"), *errorMessage);
			ShowRegisterFailureUI();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unexpected response format"));
			ShowRegisterFailureUI();
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON response"));
		ShowRegisterFailureUI();
	}
}

// 계정생성 성공 시 나타나는 UI 함수
void UKJH_LoginWidget::ShowRegisterSuccessUI()
{
	// 계정생성에 성공하는 경우, "계정이 생성되었습니다." 이미지 송출
	if (RegisterMenu_CreateSuccessImage)
	RegisterMenu_CreateSuccessImage->SetVisibility(ESlateVisibility::Visible);

	// 확인 버튼 생성
	if (RegisterMenu_SuccessFinishButton)
		RegisterMenu_SuccessFinishButton->SetVisibility(ESlateVisibility::Visible);

}

// 계정생성 성공 시 나타나는 확인 버튼 이벤트 처리 함수
void UKJH_LoginWidget::OnRegisterSuccessFinishClicked()
{
	// 계정생성 성공 시 나타나는 확인 버튼을 누를경우,
	OpenLoginMenu();

	if (RegisterMenu_CreateSuccessImage)
		RegisterMenu_CreateSuccessImage->SetVisibility(ESlateVisibility::Hidden);

	if (RegisterMenu_SuccessFinishButton)
		RegisterMenu_SuccessFinishButton->SetVisibility(ESlateVisibility::Hidden);
}

// 계정생성 실패 시 나타나는 UI 함수
void UKJH_LoginWidget::ShowRegisterFailureUI()
{
	// 계정생성에 실패하는 경우, "이미 존재하는 계정입니다." 이미지 송출
    // 0.1초 후에 UI를 표시
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        if (RegisterMenu_AlreadyExistImage)
        {
            RegisterMenu_AlreadyExistImage->SetVisibility(ESlateVisibility::Visible);
        }
        if (RegisterMenu_FailureFinishButton)
        {
            RegisterMenu_FailureFinishButton->SetVisibility(ESlateVisibility::Visible);
        }
        UE_LOG(LogTemp, Warning, TEXT("ShowRegisterFailureUI executed after delay"));
    }, 0.1f, false);
}

// 계정생성 실패 시 나타나는 확인 버튼 이벤트 처리 함수
void UKJH_LoginWidget::OnRegisterFailureFinishClicked()
{
	// 계정생성 실패 시 나타나는 확인 버튼을 누를경우,
	if (RegisterMenu_AlreadyExistImage)
		RegisterMenu_AlreadyExistImage->SetVisibility(ESlateVisibility::Hidden); // 이미지 숨김

	if (RegisterMenu_FailureFinishButton)
		RegisterMenu_FailureFinishButton->SetVisibility(ESlateVisibility::Hidden); // 버튼 숨김
}

////////// 사용자 정의형 함수 구간 - 로그인 관련 =======================================================================================================
// 로그인 요청 함수
void UKJH_LoginWidget::OnMyLogin()
{
	// 1. 입력된 정보를 가져옴
	FString LoginUserID = LoginMenu_UserIDText->GetText().ToString();
	FString LoginPassword = LoginMenu_UserPasswordText->GetText().ToString();

	// 2. 로그인 정보를 서버로 전송할 URL 설정
	FString URL = "http://125.132.216.190:7757/api/auth/login"; // 백엔드 서버 URL

	//// 3. JSON 객체 생성 후 입력된 정보 추가
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if (false == JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Create JsonObject"));
	}

	JsonObject->SetStringField("loginId", LoginUserID);
	JsonObject->SetStringField("password", LoginPassword);

	// 4. JSON 데이터를 서버가 이해할 수 있는 문자열로 변환
	FString JsonPayload; // 데이터를 담을 JsonPayload 라는 빈 문자열 생성

	// 5. JSON 데이터를 문자열로 바꾸는 도구(Writer)를 생성
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);

	// 6. JSON 객체를 문자열로 변환하여 JsonPayload에 저장
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	if (false == FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to serialize JSON object"));
		return;
	}

	// 7. HTTP 요청을 POST 방식으로 서버로 전송
	SendLoginRequest(URL, JsonPayload, "POST");
}


// 로그인 HTTP 요청을 보내는 함수
void UKJH_LoginWidget::SendLoginRequest(const FString& URL, const FString& JsonPayload, const FString& RequestType)
{
	// 1. HTTP 요청 객체를 생성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 2. 요청이 완료되면 호출될 함수 지정 (응답을 처리하는 함수 바인딩)
	Request->OnProcessRequestComplete().BindUObject(this, &UKJH_LoginWidget::OnLoginResponseReceived);

	// 3. 요청할 URL 설정
	FString RequestURL = "http://125.132.216.190:7757/api/auth/login"; // 요청을 처리할 URL 설정
	Request->SetURL(RequestURL);

	// 4. 요청 타입 설정 (Post, Get 등)
	Request->SetVerb("POST");

	// 5. 요청 헤더에 데이터를 어떤 형식으로 전송할지 설정 (JSON 등)
	Request->SetHeader("Content-Type", "application/json");

	// 6. 서버에 보낼 데이터를 담아 요청에 설정
	Request->SetContentAsString(JsonPayload);

	// 7. 설정한 요청을 서버로 전송
	Request->ProcessRequest();
}

// 로그인 HTTP 요청 결과를 처리하는 함수
void UKJH_LoginWidget::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// 1. 요청 성공과 응답성 유효에 대해 확인
	if (false == bWasSuccessful || false == Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Login request failed")); // 요청 실패 로그
		ShowLoginFailureUI();
		return;
	}

	// 2. 서버로부터 받은 응답을 문자열로 가져옴
	FString ResponseContent = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("Login Response: %s"), *ResponseContent);  // 받은 응답을 로그로 출력

	//// 새로 추가: 응답 헤더 로깅
	//TArray<FString> Headers = Response->GetAllHeaders();
	//UE_LOG(LogTemp, Log, TEXT("Response Headers:"));
	//for (const FString& Header : Headers)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("%s"), *Header);
	//}

	//// 특정 헤더 값 가져오기 (예: Authorization 토큰)
	//FString AuthToken;
	//if (Response->GetHeader(TEXT("Authorization")).IsEmpty() == false)
	//{
	//	AuthToken = Response->GetHeader(TEXT("Authorization"));
	//	UE_LOG(LogTemp, Log, TEXT("Authorization Token: %s"), *AuthToken);

	//	// GameInstance에 토큰 저장
	//	if (UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance()))
	//	{
	//		GameInstance->SetAuthToken(AuthToken);
	//		UE_LOG(LogTemp, Log, TEXT("Auth Token saved to GameInstance"));
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Authorization header not found"));
	//}

	// 3. JSON 응답을 처리하기 위해 JSON 객체 생성
	TSharedPtr<FJsonObject> JsonObject;

	// 4. 응답 내용을 Json 형식으로 읽기 위한 리더 객체 생성
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// 5. JSON 응답을 파싱(구문 해석), 성공적으로 파싱 시, JsonObject에 데이터 삽입
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		// 6. 서버 응답에서 "success" 라는 필드를 찾아, 성공 여부를 확인
		bool bSuccess = JsonObject->GetBoolField(TEXT("success"));

		if (bSuccess) // 로그인에 성공 시, 게임 인스턴스를 가져와 ServerWidgetMap으로 이동
		{
			// 로그인 성공 처리
			FString AuthToken;
			if (JsonObject->TryGetStringField(TEXT("token"), AuthToken))
			{
				// 토큰 저장 및 다음 단계로 진행
				UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance());
				if (GameInstance)
				{
					GameInstance->SetAuthToken(AuthToken);
					GameInstance->ContinueCurrentSound(); // 로그인 성공 시에도 현재 사운드 계속 재생
					GameInstance->LoadServerWidgetMap(true);
				}
			}
		}
		else
		{
			// 로그인 실패 처리
			FString errorMessage;
			if (JsonObject->TryGetStringField(TEXT("message"), errorMessage))
			{
				UE_LOG(LogTemp, Warning, TEXT("Login Failed: %s"), *errorMessage);
			}
			ShowLoginFailureUI();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unexpected response format"));
		ShowLoginFailureUI();
	}
}

// 로그인 실패 시 나타나는 UI 함수
void UKJH_LoginWidget::ShowLoginFailureUI()
{
	if (LoginMenu_InvalidAccessImage)
		LoginMenu_InvalidAccessImage->SetVisibility(ESlateVisibility::Visible);

	if (LoginMenu_InvalidAccessFinishButton)
		LoginMenu_InvalidAccessFinishButton->SetVisibility(ESlateVisibility::Visible);
}

// 로그인 실패 시 나타나는 확인 버튼 이벤트 처리 함수
void UKJH_LoginWidget::OnLoginFailureFinishClicked()
{
	// 로그인 실패 UI 숨기기
	if (LoginMenu_InvalidAccessImage)
		LoginMenu_InvalidAccessImage->SetVisibility(ESlateVisibility::Hidden);

	if (LoginMenu_InvalidAccessFinishButton)
		LoginMenu_InvalidAccessFinishButton->SetVisibility(ESlateVisibility::Hidden);
}

////////// 사용자 정의형 함수 구간 - 게스트 로그인 관련 =======================================================================================================
void UKJH_LoginWidget::OnMyGuestLogin()
{
	UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		// 현재 사운드를 유지하면서 ServerWidgetMap 로드
		GameInstance->LoadServerWidgetMap(true);

		// 사운드 계속 재생
		GameInstance->ContinueCurrentSound();
	}
}
