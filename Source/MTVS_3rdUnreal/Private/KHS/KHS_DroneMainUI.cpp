// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DroneMainUI.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UKHS_DroneMainUI::PlayCaptureAnim()
{
	if(CaputreUIAnim)
		PlayAnimation(CaputreUIAnim);
}
