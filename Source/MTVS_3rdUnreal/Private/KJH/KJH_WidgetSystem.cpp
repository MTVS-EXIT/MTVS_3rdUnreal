// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_WidgetSystem.h"

void UKJH_WidgetSystem::SetMyInterface(IKJH_Interface* Interface)
{

		this->MenuInterface = Interface;
}

void UKJH_WidgetSystem::Setup()
{

	// UI �� ��ȿ�ϴٸ�,
	if (this)
	{
		this->AddToViewport(); // Viewport �� UI�� ����
	}

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	check(PlayerController);

	if (PlayerController)
	{
		FInputModeUIOnly InputUIModeData; // UI�� ��ȣ�ۿ��� �� �� �ִ� �Է¸�带 'InputUIModeData'�� �̸����� ����

		InputUIModeData.SetWidgetToFocus(this->TakeWidget()); // ��Ŀ���� ���� ������ ����. ��, ���콺 �Է��� ServerUI ���� ������. �ٸ� ���� Ŭ�� ����.
		InputUIModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // ���콺�� Viewport ȭ�� ���� ��� �� �ִ°�? DoNotLock : ���

		PlayerController->SetInputMode(InputUIModeData); // UI ���� �Է� ��� ����. �÷��̾�� ���� ����� ��ȣ�ۿ� �Ұ��ϰ� UI�ϰ� ��ȣ�ۿ� �� �� �ְ���.
		PlayerController->bShowMouseCursor = true; // ���콺 Ŀ���� ���̰� ��.
	}

}

void UKJH_WidgetSystem::Teardown()
{
	this->RemoveFromParent(); // Viewport �� UI�� ����

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (PlayerController)
	{
		FInputModeGameOnly InputGameModeData; // Game�� ��ȣ�ۿ��� �� �� �ִ� �Է¸�带 'InputGameModeData'�� �̸����� ����

		PlayerController->SetInputMode(InputGameModeData); // �Է� ��带 ���� ���� ����
		PlayerController->bShowMouseCursor = false; // ���콺 Ŀ���� ���̰� ���� ����.
	}

}