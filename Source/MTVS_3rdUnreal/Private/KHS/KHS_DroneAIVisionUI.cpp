// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DroneAIVisionUI.h"
#include "Components/TextBlock.h"

void UKHS_DroneAIVisionUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{	
	Super::NativeTick(MyGeometry, InDeltaTime);

	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > 1.0f)
	{
		SetPercentBlock();
		CurrentTime = 0;
	}
}

void UKHS_DroneAIVisionUI::SetPercentBlock()
{
	float per = FMath::RandRange(0.1f, 9.9f);
	FString percent = FString::Printf(TEXT("%.1f"), per);
	UE_LOG(LogTemp, Warning, TEXT("percent : %f"), per);
	PercentText->SetText(FText::FromString(percent));
}
