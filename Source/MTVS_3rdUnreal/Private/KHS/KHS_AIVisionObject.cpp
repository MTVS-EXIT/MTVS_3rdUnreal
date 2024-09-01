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
	
	// 위젯 컴포넌트에서 사용자 위젯 인스턴스 가져오기
	if (WidgetComp)
	{
		UUserWidget* UserWidget = Cast<UUserWidget>(WidgetComp->GetUserWidgetObject());
		if (UserWidget)
		{
			// UserWidget을 통해 원하는 작업 수행
			UserWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

// Called every frame
void AKHS_AIVisionObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어 카메라 매니저 가져오기
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager)
	{
		return;
	}

	// 카메라 위치 가져오기
	FVector CameraLocation = CameraManager->GetCameraLocation();

	// 위젯 씬 컴포넌트의 월드 위치 가져오기
	FVector WidgetLocation = WidgetComp->GetComponentLocation(); 

	// 위젯에서 카메라로의 방향 벡터 계산
	FVector Direction = CameraLocation - WidgetLocation;
	Direction.Normalize();  // 벡터를 정규화하여 방향을 얻음

	// 방향 벡터로부터 회전 생성
	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	// 위젯 씬 컴포넌트의 월드 회전 설정
	WidgetComp->SetWorldRotation(NewRotation); 
}

