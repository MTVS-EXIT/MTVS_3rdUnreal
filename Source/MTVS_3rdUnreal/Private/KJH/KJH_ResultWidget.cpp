// Fill out your copyright notice in the Description page of Project Settings.

#include "KJH/KJH_ResultWidget.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "KJH/KJH_GameInstance.h"
#include "KJH/KJH_PlayerState.h"  
#include "HttpModule.h"                  
#include "Interfaces/IHttpRequest.h"     
#include "Interfaces/IHttpResponse.h"           

bool UKJH_ResultWidget::Initialize()
{
	Super::Initialize();
	
    if (CommonResultMenu_QuitButton)
        CommonResultMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_ResultWidget::QuitPressed); // Quit 버튼 눌렀을 때 QuitPressed 함수 호출
    
    if (CommonButton_Report)
        CommonButton_Report->OnClicked.AddDynamic(this, &UKJH_ResultWidget::MoveToReportWeb); // CommonButton_Report 버튼 눌렀을 때 MoveToReportWeb 함수 호출

    if (DroneResultMenu_PersonResultButton)
        DroneResultMenu_PersonResultButton->OnClicked.AddDynamic(this, &UKJH_ResultWidget::SwitchToPersonResultMenu); // PersonResultButton 버튼 눌렀을 때 SwitchToPersonResultMenu 함수 호출

    if (PersonResultMenu_DroneResultButton)
        PersonResultMenu_DroneResultButton->OnClicked.AddDynamic(this, &UKJH_ResultWidget::SwitchToDroneResultMenu); // DroneResultButton 버튼 눌렀을 때 SwitchToDroneResultMenu 함수 호출

	return true;
}

////////// 사용자 정의형 함수 구간 - 버튼 함수 관련 ==============================================================================
void UKJH_ResultWidget::QuitPressed()
{
    if (MenuInterface)
    {
        MenuInterface->LoadServerWidgetMap(false); // false 인자를 통해 현재 사운드를 유지하지 않으며 이동
        Teardown(); // Widget 파괴
    }
}

// 소방관 결과 메뉴로 전환 함수
void UKJH_ResultWidget::SwitchToPersonResultMenu()
{
    if (MenuSwitcher)
        MenuSwitcher->SetActiveWidget(PersonResultMenu);

    PlayAnimation(ShowPersonResultAnim);
}

// 드론 결과 메뉴로 전환 함수
void UKJH_ResultWidget::SwitchToDroneResultMenu()
{
    if (MenuSwitcher)
        MenuSwitcher->SetActiveWidget(DroneResultMenu);

    PlayAnimation(ShowDroneResultAnim);
}

////////// 사용자 정의형 함수 구간 - 위젯 애니메이션 관련 ==============================================================================
// 
void UKJH_ResultWidget::PlayResultAnimations()
{

    if (ShowRescueAnim)
    {
        UE_LOG(LogTemp, Warning, TEXT("Playing ShowRescueAnim"));
        PlayAnimation(ShowRescueAnim);

        // ShowRescueAnim의 길이만큼 대기 후 ShowCommonResultAnim 실행
        FTimerHandle TimerHandle;
        float AnimationDuration = ShowRescueAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UKJH_ResultWidget::OnRescueAnimationFinished, AnimationDuration, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowRescueAnim not found, moving to ShowCommonResultAnim"));
        OnRescueAnimationFinished();
    }
}

void UKJH_ResultWidget::OnRescueAnimationFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("ShowRescueAnim finished, playing ShowCommonResultAnim"));

    if (ShowCommonResultAnim)
        PlayAnimation(ShowCommonResultAnim);

    else
        UE_LOG(LogTemp, Error, TEXT("ShowCommonResultAnim not found"));
}

// 결과값 저장 함수
void UKJH_ResultWidget::SetResultData(int32 PersonSearchRoomResult, int32 PersonItemUsedResult, int32 PersonDamageResult, 
                                      int32 DroneDetectedResult, int32 DroneDetectedSafeResult, int32 DroneDetectedCautionResult, int32 DroneDetectedDangerResult)
{
    // 포맷 함수 정의
    auto FormatNumber = [](int32 Number) -> FString {
        return FString::Printf(TEXT("(%02d)"), Number);
        };

    //// 포맷 함수 정의 방법 2
    //auto FormatNumber = [](int32 Number) -> FText {
    //    return FText::Format(INVTEXT("({0:02})"), FText::AsNumber(Number));

    // 소방관 플레이 결과값 저장
    //if (PersonResultMenu_FindRoomNum)
    //    PersonResultMenu_FindRoomNum->SetText(FText::AsNumber(PersonSearchRoomResult));

    //if (PersonResultMenu_FindItemNum)
    //    PersonResultMenu_FindItemNum->SetText(FText::AsNumber(PersonItemUsedResult));

    //if (PersonResultMenu_DamageNum)
    //    PersonResultMenu_DamageNum->SetText(FText::AsNumber(PersonDamageResult));
    if (PersonResultMenu_FindRoomNum)
        PersonResultMenu_FindRoomNum->SetText(FText::FromString(FormatNumber(PersonSearchRoomResult)));

    if (PersonResultMenu_FindItemNum)
        PersonResultMenu_FindItemNum->SetText(FText::FromString(FormatNumber(PersonItemUsedResult)));

    if (PersonResultMenu_DamageNum)
        PersonResultMenu_DamageNum->SetText(FText::FromString(FormatNumber(PersonDamageResult)));

    // 드론 플레이 결과값 저장
    //if (DroneResultMenu_DetectedNum)
    //    DroneResultMenu_DetectedNum->SetText(FText::AsNumber(DroneDetectedResult));

    //if (DroneResultMenu_DangerDetectedNum)
    //    DroneResultMenu_DangerDetectedNum->SetText(FText::AsNumber(DroneDetectedDangerResult));

    //if (DroneResultMenu_CautionDetectedNum)
    //    DroneResultMenu_CautionDetectedNum->SetText(FText::AsNumber(DroneDetectedCautionResult));

    //if (DroneResultMenu_SafeDetectedNum)
    //    DroneResultMenu_SafeDetectedNum->SetText(FText::AsNumber(DroneDetectedSafeResult));
    if (DroneResultMenu_DetectedNum)
        DroneResultMenu_DetectedNum->SetText(FText::FromString(FormatNumber(DroneDetectedResult)));

    if (DroneResultMenu_DangerDetectedNum)
        DroneResultMenu_DangerDetectedNum->SetText(FText::FromString(FormatNumber(DroneDetectedDangerResult)));

    if (DroneResultMenu_CautionDetectedNum)
        DroneResultMenu_CautionDetectedNum->SetText(FText::FromString(FormatNumber(DroneDetectedCautionResult)));

    if (DroneResultMenu_SafeDetectedNum)
        DroneResultMenu_SafeDetectedNum->SetText(FText::FromString(FormatNumber(DroneDetectedSafeResult)));
}

void UKJH_ResultWidget::MoveToReportWeb()
{
    UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance"));
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController"));
        return;
    }

    AKJH_PlayerState* PlayerState = PC->GetPlayerState<AKJH_PlayerState>();
    if (!PlayerState)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerState"));
        return;
    }

    // 인증 토큰 가져오기 및 공백 제거
    FString AuthToken = GameInstance->GetAuthToken();
    AuthToken = AuthToken.TrimStartAndEnd();  // 앞뒤 공백 제거
    AuthToken = AuthToken.Replace(TEXT(" "), TEXT(""));  // 중간 공백 제거

    UE_LOG(LogTemp, Log, TEXT("Auth Token: %s"), *AuthToken);

    if (AuthToken.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Auth token is empty. Cannot proceed with data submission."));
        if (GEngine)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("인증 정보가 없습니다. 다시 로그인해 주세요."));
        }
        return;
    }

    // JSON 객체 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // 플레이어 데이터 추가
    if (PlayerState->bIsPersonCharacterSelected)
    {
        JsonObject->SetNumberField("room", PlayerState->PersonState_SearchRoomCount);
        JsonObject->SetNumberField("item", PlayerState->PersonState_ItemUsedCount);
        JsonObject->SetNumberField("damage", PlayerState->PersonState_DamageCount);
    }
    else
    {
        JsonObject->SetNumberField("detection", PlayerState->DroneState_DetectedCount);
        JsonObject->SetNumberField("safe", PlayerState->DroneState_DetectedSafeCount);
        JsonObject->SetNumberField("caution", PlayerState->DroneState_DetectedCautionCount);
        JsonObject->SetNumberField("danger", PlayerState->DroneState_DetectedDangerCount);
    }

    // JSON을 문자열로 변환
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // JSON 데이터 로그 출력 (디버깅)
    UE_LOG(LogTemp, Log, TEXT("JSON Sent: %s"), *JsonString);

    // HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    // URL 설정
    FString URL;
    if (PlayerState->bIsPersonCharacterSelected)
    {
        URL = "http://125.132.216.190:7757/api/rank";
    }
    else
    {
        URL = "http://125.132.216.190:7757/api/drank";
    }
    Request->SetURL(URL);

    // POST 방식 및 헤더 설정
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));

    Request->SetContentAsString(JsonString);

    // 요청 완료 시 호출될 함수 설정
    Request->OnProcessRequestComplete().BindUObject(this, &UKJH_ResultWidget::OnReportDataSent);

    // 요청 전송
    Request->ProcessRequest();

    UE_LOG(LogTemp, Log, TEXT("Sending data to server. URL: %s, Data: %s"), *URL, *JsonString);
}

void UKJH_ResultWidget::OnReportDataSent(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        int32 ResponseCode = Response->GetResponseCode();
        FString ResponseContent = Response->GetContentAsString();

        // 서버 응답 코드 및 내용 로그 출력
        UE_LOG(LogTemp, Log, TEXT("Server Response Code: %d"), ResponseCode);
        UE_LOG(LogTemp, Log, TEXT("Server Response Content: %s"), *ResponseContent);

        if (ResponseCode == 200 || ResponseCode == 201)  // 성공적인 응답 처리
        {
            UE_LOG(LogTemp, Log, TEXT("Data sent successfully to server."));

            if (GEngine)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Data Sent Successfully!"));
            }

            // 리더보드 웹 페이지로 이동
            FPlatformProcess::LaunchURL(TEXT("http://125.132.216.190:7758/"), nullptr, nullptr);
        }
        else if (ResponseCode == 401 || ResponseCode == 403)  // 인증 오류 처리
        {
            UE_LOG(LogTemp, Warning, TEXT("Authentication failed. Please log in again. Response Code: %d"), ResponseCode);
            if (GEngine)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Authorization Failed"));
            }
        }
        else  // 기타 서버 오류 처리
        {
            UE_LOG(LogTemp, Warning, TEXT("Server error occurred. Response code: %d"), ResponseCode);
            if (GEngine)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Server Error: %d"), ResponseCode));
            }
        }
    }
    else  // 요청 실패 처리
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Request failed or response was invalid."));
        if (GEngine)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Data Sent Failed"));
        }
    }
}