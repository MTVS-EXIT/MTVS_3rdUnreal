// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_WidgetSystem.h"

void UKJH_WidgetSystem::SetMyInterface(IKJH_Interface* Interface)
{

		this->MenuInterface = Interface;
}

void UKJH_WidgetSystem::Setup()
{
	// UI가 유효하다면 Viewport에 추가
	if (this)
		this->AddToViewport();

	// GetWorld()가 유효한지 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is not valid in Setup."));
		return; // World가 유효하지 않으면 함수를 종료
	}

	// PlayerController가 유효한지 확인
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid in Setup."));
		return; // PlayerController가 유효하지 않으면 함수를 종료
	}

	if (PlayerController)
	{
		// UI 전용 입력 모드 설정
		FInputModeUIOnly InputUIModeData; // UI와 상호작용을 할 수 있는 입력모드를 'InputUIModeData'란 이름으로 설정

		InputUIModeData.SetWidgetToFocus(this->TakeWidget()); // 포커스를 받을 위젯을 설정. 즉, 마우스 입력은 ServerUI 에만 가능함. 다른 곳은 클릭 막음.
		InputUIModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스가 Viewport 화면 밖을 벗어날 수 있는가? DoNotLock : 허용

		PlayerController->SetInputMode(InputUIModeData); // UI 전용 입력 모드 적용. 플레이어는 게임 월드와 상호작용 불가하고 UI하고만 상호작용 할 수 있게함.
		PlayerController->bShowMouseCursor = true; // 마우스 커서를 보이게 함.
	}
}

void UKJH_WidgetSystem::Teardown()
{
	// GetWorld()가 유효한지 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is not valid in Teardown."));
		return; // World가 유효하지 않으면 함수를 종료
	}

	// PlayerController가 유효한지 확인
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid in Teardown."));
		return; // PlayerController가 유효하지 않으면 함수를 종료
	}

	if (PlayerController)
	{
		// 게임 전용 입력 모드 설정
		FInputModeGameOnly InputGameModeData; // Game과 상호작용을 할 수 있는 입력모드를 'InputGameModeData'란 이름으로 설정
		PlayerController->SetInputMode(InputGameModeData); // 입력 모드를 게임 모드로 설정
		PlayerController->bShowMouseCursor = false; // 마우스 커서를 보이게 하지 않음.
	}

	// Viewport에서 UI 제거
	if (this->IsInViewport())
		this->RemoveFromParent();
}