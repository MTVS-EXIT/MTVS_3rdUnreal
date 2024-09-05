// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
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

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// 시계 보기 (Q)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WatchAction;

	// 도끼 모드 -> 추후 주웠을때로 변경

	// R - 도끼 찍기, 소화기 던지기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RAction;

	
	
public:
	// Sets default values for this character's properties
	AJSH_Player();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	void Watch(const FInputActionValue& Value);

	void R(const FInputActionValue& Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return Camera; }

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Network Bool Replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	// 시계

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


	// 도끼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool AxModeON = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool AxAnimationplay = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AxAnimations")
	UAnimMontage* AxMontage;

	UFUNCTION(Server, Reliable)
	void Server_RAction();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_RAction();
	
};
