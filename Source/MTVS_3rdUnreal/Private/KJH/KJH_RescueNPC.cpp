// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_RescueNPC.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "KJH/KJH_GameModeBase.h"
#include "JSH/JSH_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KJH/KJH_PlayerState.h"

// Sets default values
AKJH_RescueNPC::AKJH_RescueNPC()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 상호작용 구체 생성 및 설정
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSpherec"));
	SetRootComponent(InteractionSphere);
	InteractionSphere->SetSphereRadius(200.0f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(InteractionSphere);

	// RPC 설정
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AKJH_RescueNPC::BeginPlay()
{
	Super::BeginPlay();
	
	// Sphere에 Overlap 시 호출되는 델리게이트 함수
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AKJH_RescueNPC::OnInteractionSphereOverlapBegin);
}

// Called every frame
void AKJH_RescueNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKJH_RescueNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

////////// BeginOverlap 이벤트 함수 구간 ==============================================================================
void AKJH_RescueNPC::OnInteractionSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AJSH_Player* HumanPlayer = Cast<AJSH_Player>(OtherActor);
	if (HumanPlayer)
		Server_NotifyPlayerContact(HumanPlayer); // 서버에 정보를 전달
}

void AKJH_RescueNPC::Server_NotifyPlayerContact_Implementation(AJSH_Player* ContactPlayer)
{
	if (HasAuthority())
	{
		if (ContactPlayer)
		{
			ContactPlayer->GetCharacterMovement()->StopMovementImmediately();
			ContactPlayer->GetCharacterMovement()->DisableMovement();

			// PlayerState를 통해 게임 종료 처리
			AKJH_PlayerState* PS = ContactPlayer->GetPlayerState<AKJH_PlayerState>();
			if (PS)
			{
				PS->Multicast_TriggerGameEnd();
			}
		}
		InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionSphere->SetSimulatePhysics(false);
	}
}

