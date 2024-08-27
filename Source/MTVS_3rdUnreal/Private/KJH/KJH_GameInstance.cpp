// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_GameInstance.h"

UKJH_GameInstance::UKJH_GameInstance(const FObjectInitializer& ObjectInitializer) // ������. ������ ������ �� �߻�.
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));
}

void UKJH_GameInstance::Init() // �÷��̸� ������ ���� �߻�
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

void UKJH_GameInstance::Host()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Hosting"));
}
