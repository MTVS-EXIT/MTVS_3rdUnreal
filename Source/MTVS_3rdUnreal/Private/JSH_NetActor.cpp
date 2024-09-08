// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_NetActor.h"

#include "EngineUtils.h"
#include "JSH/JSH_Player.h"

// Sets default values
AJSH_NetActor::AJSH_NetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	

}

// Called when the game starts or when spawned
void AJSH_NetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJSH_NetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	FindOwner();
	
}

void AJSH_NetActor::FindOwner()
{
	if ( HasAuthority() )
	{
		AActor* newOwner = nullptr;
		float minDist = SearchDistance;

		for ( TActorIterator<AJSH_Player> it(GetWorld()); it; ++it )
		{
			AActor* otherActor = *it;
			float dist = GetDistanceTo(otherActor);

			if ( dist < SearchDistance )
			{
				minDist = dist;
				newOwner = otherActor;
			}
		}
		// Owner 설정
		if ( newOwner != GetOwner() )
		{
			SetOwner(newOwner);
		}
	}
}

