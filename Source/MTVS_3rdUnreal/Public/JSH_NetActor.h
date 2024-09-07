// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_NetActor.generated.h"

UCLASS()
class MTVS_3RDUNREAL_API AJSH_NetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJSH_NetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComp;

	// Owner 검출 영역
	UPROPERTY(EditAnywhere)
	float SearchDistance = 150;

	// Owner 설정
	void FindOwner();

};
