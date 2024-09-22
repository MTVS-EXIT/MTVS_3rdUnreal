// Fill out your copyright notice in the Description page of Project Settings.

#include "KJH/KJH_ResultWidget.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"

bool UKJH_ResultWidget::Initialize()
{
	Super::Initialize();
	
	return true;
}

////////// 사용자 정의형 함수 구간 ==============================================================================
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
    {
        PlayAnimation(ShowCommonResultAnim);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ShowCommonResultAnim not found"));
    }
}
