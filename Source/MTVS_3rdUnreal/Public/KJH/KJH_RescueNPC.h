// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KJH_RescueNPC.generated.h"

UCLASS()
class MTVS_3RDUNREAL_API AKJH_RescueNPC : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKJH_RescueNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


////////// 기본 뼈대 구간 ============================================================================================
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    class USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    class USphereComponent* InteractionSphere;

////////// BeginOverlap 이벤트 함수 구간 ==============================================================================
	UFUNCTION()
    void OnInteractionSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

////////// 사용자 정의형 함수 구간 ==============================================================================
	UFUNCTION(Server, Reliable)
    void Server_NotifyPlayerContact(AJSH_Player* ContactPlayer); // 서버에게 플레이어가 접촉했음을 알리는 함수

};
