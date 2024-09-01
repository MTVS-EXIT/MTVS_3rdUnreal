// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_AIVisionObject.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "kismet/GameplayStatics.h"

// Sets default values
AKHS_AIVisionObject::AKHS_AIVisionObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);


	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetRelativeScale3D(FVector(0.15f));

}

// Called when the game starts or when spawned
void AKHS_AIVisionObject::BeginPlay()
{
	Super::BeginPlay();
	
	// ���� ������Ʈ���� ����� ���� �ν��Ͻ� ��������
	if (WidgetComp)
	{
		UUserWidget* UserWidget = Cast<UUserWidget>(WidgetComp->GetUserWidgetObject());
		if (UserWidget)
		{
			// UserWidget�� ���� ���ϴ� �۾� ����
			UserWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

// Called every frame
void AKHS_AIVisionObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �÷��̾� ī�޶� �Ŵ��� ��������
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager)
	{
		return;
	}

	// ī�޶� ��ġ ��������
	FVector CameraLocation = CameraManager->GetCameraLocation();

	// ���� �� ������Ʈ�� ���� ��ġ ��������
	FVector WidgetLocation = WidgetComp->GetComponentLocation(); 

	// �������� ī�޶���� ���� ���� ���
	FVector Direction = CameraLocation - WidgetLocation;
	Direction.Normalize();  // ���͸� ����ȭ�Ͽ� ������ ����

	// ���� ���ͷκ��� ȸ�� ����
	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	// ���� �� ������Ʈ�� ���� ȸ�� ����
	WidgetComp->SetWorldRotation(NewRotation); 
}

