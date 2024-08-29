// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_UserWidget.h"
#include "Components/Button.h"

void UKJH_UserWidget::NativeConstruct()
{

}

bool UKJH_UserWidget::Initialize()
{
	Super::Initialize();

	if (HostButton)
	{
	HostButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::HostServer); // ��ư ������ �� HostServer �Լ� ȣ��
	}

	return true;

}

void UKJH_UserWidget::SetMyInterface(IKJH_Interface* Interface)
{
	this -> MyInterface = Interface;
}

////////// ����� ������ �Լ� ���� -----------------------------------------------------
void UKJH_UserWidget::HostServer()
{
	if (MyInterface)
	{
		MyInterface->Host();
	}
}
