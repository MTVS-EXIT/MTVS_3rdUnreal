// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_WidgetSystem.h"

void UKJH_WidgetSystem::SetMyInterface(IKJH_Interface* Interface)
{

		this->MenuInterface = Interface;
}

void UKJH_WidgetSystem::Setup()
{
	// UI�� ��ȿ�ϴٸ� Viewport�� �߰�
	if (this)
	{
		this->AddToViewport();
	}

	// GetWorld()�� ��ȿ���� Ȯ��
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is not valid in Setup."));
		return; // World�� ��ȿ���� ������ �Լ��� ����
	}

	// PlayerController�� ��ȿ���� Ȯ��
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid in Setup."));
		return; // PlayerController�� ��ȿ���� ������ �Լ��� ����
	}

	if (PlayerController)
	{
		// UI ���� �Է� ��� ����
		FInputModeUIOnly InputUIModeData; // UI�� ��ȣ�ۿ��� �� �� �ִ� �Է¸�带 'InputUIModeData'�� �̸����� ����

		InputUIModeData.SetWidgetToFocus(this->TakeWidget()); // ��Ŀ���� ���� ������ ����. ��, ���콺 �Է��� ServerUI ���� ������. �ٸ� ���� Ŭ�� ����.
		InputUIModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // ���콺�� Viewport ȭ�� ���� ��� �� �ִ°�? DoNotLock : ���

		PlayerController->SetInputMode(InputUIModeData); // UI ���� �Է� ��� ����. �÷��̾�� ���� ����� ��ȣ�ۿ� �Ұ��ϰ� UI�ϰ� ��ȣ�ۿ� �� �� �ְ���.
		PlayerController->bShowMouseCursor = true; // ���콺 Ŀ���� ���̰� ��.
	}

}

void UKJH_WidgetSystem::Teardown()
{
	// GetWorld()�� ��ȿ���� Ȯ��
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is not valid in Teardown."));
		return; // World�� ��ȿ���� ������ �Լ��� ����
	}

	// PlayerController�� ��ȿ���� Ȯ��
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid in Teardown."));
		return; // PlayerController�� ��ȿ���� ������ �Լ��� ����
	}

	if (PlayerController)
	{
		// ���� ���� �Է� ��� ����
		FInputModeGameOnly InputGameModeData; // Game�� ��ȣ�ۿ��� �� �� �ִ� �Է¸�带 'InputGameModeData'�� �̸����� ����

		PlayerController->SetInputMode(InputGameModeData); // �Է� ��带 ���� ���� ����
		PlayerController->bShowMouseCursor = false; // ���콺 Ŀ���� ���̰� ���� ����.
	}

	// Viewport���� UI ����
	if (this->IsInViewport())
	{
		this->RemoveFromParent();
	}
}