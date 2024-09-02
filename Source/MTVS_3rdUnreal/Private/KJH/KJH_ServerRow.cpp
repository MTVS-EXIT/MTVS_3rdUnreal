// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_ServerRow.h"
#include "KJH/KJH_ServerWidget.h"
#include "Components/Button.h"

void UKJH_ServerRow::Setup(class UKJH_ServerWidget* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
	RowButton->OnClicked.AddDynamic(this, &UKJH_ServerRow::OnClicked);
}

void UKJH_ServerRow::OnClicked()
{
	Parent->SelecetIndex(Index);
}
