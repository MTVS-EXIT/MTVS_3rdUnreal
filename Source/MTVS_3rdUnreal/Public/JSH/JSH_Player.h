// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "JSH_Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class MTVS_3RDUNREAL_API AJSH_Player : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* TwinSkeletal;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// 시계 보기 (Q)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WatchAction;

	// 물건 줍기 (F)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrabAction;

	// LeftClick - 도끼 찍기, 소화기 던지기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReadyAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PushAction;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// UInputAction* SlideAction;
	
public:
	AJSH_Player();

protected:
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
	
	void Watch(const FInputActionValue& Value);

	void LeftMouse(const FInputActionValue& Value);

	void Walk(const FInputActionValue& Value);

	void Grab(const FInputActionValue& Value);
	
	void R(const FInputActionValue& Value);

	void E(const FInputActionValue& Value);

	// void Slide(const FInputActionValue& Value);

protected:
	virtual void BeginPlay() override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return Camera; }

	
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	// * 시계 ==================================

	UPROPERTY(EditAnywhere, Category=Watch)
	UChildActorComponent* DigitalWatch;

	UPROPERTY(EditAnywhere, Category=Watch)
	UWidgetComponent* WatchWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WatchAnimations")
	UAnimMontage* WatchMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WatchAnimations")
	UAnimMontage* WatchReverseMontage;
	
	UPROPERTY(Replicated)
	bool WatchSee = true;
	
	UFUNCTION(Server, Reliable)
	void Server_WatchSee();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_WatchSee();
	// ================================== {시계}


	
	// * 손전등 ========================================
	UPROPERTY(EditAnywhere, Category=FlashLight)
	UChildActorComponent* FlashLightChildActor;

	UPROPERTY(EditAnywhere, Category=FlashLight)
	USpotLightComponent* FlashLight;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool FlashON = false;
	//  ======================================== {손전등}



	
	// * 방독면 =======================================
	UPROPERTY(EditAnywhere, Category=GassMask)
	UChildActorComponent* GassMask;

	UPROPERTY(Replicated)
	bool GassMaskOn = false;
	
	UPROPERTY()
	TArray<AActor*> GMList;

	// 소유한 방독면
	UPROPERTY()
	class AActor* GrabGMActor;
	// ======================================= {방독면}
	


	// * Ready Mode ===================
	UFUNCTION(Server, Reliable)
	void Server_RedyAction();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_RedyAction();
	//  =================== {Ready Mode}


	
	// * 잡기(f) ======================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool BHasAX;

	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite, Replicated)
	bool AXOn;

	// 태어날 때 모든 목록을 기억
	UPROPERTY()
	TArray<AActor*> AXList;

	// 잡았을 때 위치
	UPROPERTY(EditDefaultsOnly , Category = Pistol)
	class USceneComponent* HandComp;

	// 소유한 총을 기억.
	UPROPERTY()
	class AActor* GrabAXActor;

	UPROPERTY(EditDefaultsOnly , Category = Pistol)
	float GrabDistance = 150;

	void AttachAX(AActor* AXActor);

	void DetachAX(AActor* AXActor);
	void MyTakeAX();
	void MyReleaseAX();

	UFUNCTION(Server, Reliable)
	void Server_Grab();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Grab();
	// =============================================== {잡기(f)}



	

	// * FireEX ===========================================
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite, Replicated)
	bool BHasFire;

	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite, Replicated)
	bool FireEXOn = false;
	
	UPROPERTY()
	TArray<AActor*> FireList;
	
	UPROPERTY(EditDefaultsOnly , Category = Pistol)
	class USceneComponent* FireHandComp;
	
	UPROPERTY()
	class AActor* GrabFireActor;
	
	void AttachFire(AActor* FireActor);
	void DetachFire(AActor* FireActor);
	void MyTakeFire();
	void MyReleaseFire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* FireEXSpray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* FireEXNiagara;


	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite, Replicated)
	bool FireEXSprayOnBool = false;
	
	void FireEXSprayTrace(float DeltaTime);
	
	float currtime = 0;
	float spraytime = 5;
	// =========================================== {FireEX}



	
	
	// * 도끼 =============================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool AxModeON = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool AxAnimationplay = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AxAnimations")
	UAnimMontage* AxMontage;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ax")
	// UStaticMeshComponent* AX;

	UFUNCTION(Server, Reliable)
	void Server_LeftMouseAction();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_LeftMouseAction();
	// =============================================={도끼}



	
	// * 걷기 ===============================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool WantWalk = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool WantSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool PossibleWalk = true;

	UFUNCTION(Server, Reliable)
	void Server_Walk();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Walk();
	//=============================================== {걷기}



	// * Push Actor =================================================
	// ** Key: E
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool PushMode = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool PushPossible= false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	class UCapsuleComponent* PushCapsule;
	
	UFUNCTION(Server, Reliable)
	void Server_E();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_E();
	
	// ================================================= {Push Actor}

	// 모든 인터렉션 끄기
	void AllOff();

	// Slide
	// UFUNCTION(Server, Reliable)
	// void Server_E();
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void NetMulti_E();
	//
	// // Timer Handle for controlling the timer
	// FTimerHandle TimerHandle;
	//
	// // Function to be called after the timer expires
	// void TimerFinished();
};
