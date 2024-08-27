// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_GameInstance.h"

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // 생성자. 에디터 실행할 때 발생.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // 플레이를 눌렀을 때만 발생
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

void UKJH_GameInstance::Host()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Hosting"));
}
