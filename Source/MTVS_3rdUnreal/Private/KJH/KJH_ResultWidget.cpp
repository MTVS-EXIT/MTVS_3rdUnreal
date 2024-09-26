// Fill out your copyright notice in the Description page of Project Settings.

#include "KJH/KJH_ResultWidget.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

bool UKJH_ResultWidget::Initialize()
{
	Super::Initialize();
	
    if (CommonResultMenu_QuitButton)
        CommonResultMenu_QuitButton->OnClicked.AddDynamic(this, &UKJH_ResultWidget::QuitPressed); // Quit 버튼 눌렀을 때 QuitPressed 함수 호출
    
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
        MenuInterface->LoadServerWidgetMap(false); // ServerWidget맵으로 이동
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
