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
	HostButton->OnClicked.AddDynamic(this, &UKJH_UserWidget::HostServer); // 버튼 눌렀을 때 HostServer 함수 호출
	}

	return true;

}

void UKJH_UserWidget::SetMyInterface(IKJH_Interface* Interface)
{
	this -> MyInterface = Interface;
}

////////// 사용자 정의형 함수 구간 -----------------------------------------------------
void UKJH_UserWidget::HostServer()
{
	if (MyInterface)
	{
		MyInterface->Host();
	}
}
