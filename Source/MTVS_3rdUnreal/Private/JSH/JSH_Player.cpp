﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH/JSH_Player.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/ArrowComponent.h"
#include "Components/SpotLightComponent.h"
#include "JSH_PlayerMainUI.h"
#include "kismet/GameplayStatics.h"
#include "KJH/KJH_PlayerController.h"
#include "KJH/KJH_PlayerState.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


AJSH_Player::AJSH_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(38.f, 96.0f);
	
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin.SKM_UEFN_Mannequin'"));
    if (TMesh.Succeeded())
    {
	    GetMesh()->SetSkeletalMesh(TMesh.Object);
    	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
    }
	
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 


	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


	TwinSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TwinSkeletal"));
	TwinSkeletal->SetupAttachment(GetMesh());
	TwinSkeletal->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TTMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Personnel/Meshes/Clothes/SK_BioWorker.SK_BioWorker'"));
	// if (TTMesh.Succeeded())
	// {
	// 	GetMesh()->SetSkeletalMesh(TTMesh.Object);
	// 	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	// }

	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), FName("neck_02"));
	SpringArm->SetRelativeLocation(FVector(10.0f, 23.5f, -2.0f));
	// SpringArm->SetupAttachment(GetMesh(), FName("VB Vhead_root"));
	// SpringArm->SetRelativeLocation(FVector(0, 23.5f, 168));
	// SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("pelvis"));
	SpringArm->TargetArmLength = 0.f; 
	SpringArm->bUsePawnControlRotation = true; 


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; 




	DigitalWatch = CreateDefaultSubobject<UChildActorComponent>(TEXT("DigitalWatch"));
	DigitalWatch->SetupAttachment(TwinSkeletal, FName("lowerarm_twist_01_l"));
	DigitalWatch->SetRelativeLocation(FVector(14.226082f, 0.685825f, 0.382135f));
	DigitalWatch->SetRelativeRotation(FRotator(13.876860f, -0.433584f, -59.389724f));
	DigitalWatch->SetRelativeScale3D(FVector(1.15f, 1.15f, 1.15f));
	
	
	WatchWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WatchWidget"));
	WatchWidget->SetupAttachment(DigitalWatch);
	WatchWidget->SetRelativeLocationAndRotation(FVector(-0.063780f, 2.781286f, 0.11564f), FRotator(0.f, 90.0f, 0.f));
	WatchWidget->SetRelativeScale3D(FVector(0.0013f, 0.003f, 0.003f));



	FlashLightChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("FlashLightChildActor"));
	FlashLightChildActor->SetupAttachment(TwinSkeletal, FName("spine_03"));
	FlashLightChildActor->SetRelativeLocation(FVector(-3.098716f, 13.331717f, -14.229190f));
	FlashLightChildActor->SetRelativeRotation(FRotator(-1.688164f, -94.706464f, 80.203971f));
	FlashLightChildActor->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));
	
	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	FlashLight->SetupAttachment(TwinSkeletal, FName("spine_03"));
	FlashLight->SetRelativeLocation(FVector(7.721326f, 16.548045f, -11.446471f));
	FlashLight->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	FlashLight->SetRelativeScale3D(FVector(1.111111f, 1.111111f, 1.111111f));
	FlashLight->SetVisibility(false);
	
	GassMask = CreateDefaultSubobject<UChildActorComponent>(TEXT("GassMask"));
	GassMask->SetupAttachment(GetMesh(), FName("head"));
	GassMask->SetRelativeLocation(FVector(-2.608696f, 1.521739f, 0.304348f));
	GassMask->SetRelativeRotation(FRotator(90.000000f, 1440.000000f, 1260.000001f));
	GassMask->SetVisibility(false);

	
	// AX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AX"));
	// AX->SetupAttachment(GetMesh(), FName("AX"));
	// AX->SetRelativeLocation(FVector(-80.147944f, -916.095325f, 95.000000f));
	// AX->SetRelativeRotation(FRotator(0.f, 5.000001f, 0.f));
	// AX->SetVisibility(false);


	// HandComp = CreateDefaultSubobject<USceneComponent>(TEXT("HandComp"));
	// HandComp->SetupAttachment(GetMesh() , TEXT("GrabPosition"));
	// // HandComp->SetRelativeLocationAndRotation(FVector(-17.586273f , -0.273735f , 15.693467f) , FRotator(-62.920062f , 9.732144f , 29.201706f));

	HandComp = CreateDefaultSubobject<USceneComponent>(TEXT("HandComp"));
	HandComp->SetupAttachment(TwinSkeletal , TEXT("Bio_AX"));
	
		
	FireHandComp = CreateDefaultSubobject<USceneComponent>(TEXT("FireHandComp"));
	FireHandComp->SetupAttachment(TwinSkeletal , TEXT("Bio_FirePosition"));

	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);


	// FireEXSpray = CreateDefaultSubobject<UArrowComponent>(TEXT("FireEXSpray"));
	// FireEXSpray->SetupAttachment(GetMesh(), TEXT("FirePosition"));
	// FireEXSpray->SetRelativeLocation(FVector(-50.210770f, 0.279998f, 13.0f));
	// FireEXSpray->SetRelativeRotation(FRotator(0.0f, 174.999999f, 0.0f));
	FireEXSpray = CreateDefaultSubobject<UArrowComponent>(TEXT("FireEXSpray"));
	FireEXSpray->SetupAttachment(TwinSkeletal, TEXT("FirePosition"));
	FireEXSpray->SetRelativeLocation(FVector(-50.210770f, 0.279998f, 13.0f));
	FireEXSpray->SetRelativeRotation(FRotator(0.0f, 174.999999f, 0.0f));


	
	FireEXNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEXNiagara"));
	FireEXNiagara->SetupAttachment(TwinSkeletal, TEXT("Bio_FirePosition"));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempFireEX(TEXT("/Script/Niagara.NiagaraSystem'/Game/MAPS/TA_NYH/Fire_Extinguisher_And_Effect/NS_Fire_Extinguisher.NS_Fire_Extinguisher'"));
	if (TempFireEX.Succeeded())
	{
		FireEXNiagara->SetAsset(TempFireEX.Object);
	}
	FireEXNiagara->SetRelativeLocation(FVector(-45.439986f, -5.781532f, 18.888889f));
	FireEXNiagara->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	FireEXNiagara->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	FireEXNiagara->SetAutoActivate(false);
	
	
	PushCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PushCapsule"));
	PushCapsule->SetupAttachment(TwinSkeletal);
	PushCapsule->SetRelativeLocation(FVector(0.0f, 93.333336f, 131.111115f));
	PushCapsule->SetRelativeRotation(FRotator(90.0f, -0.007734f, -0.007734f));
	PushCapsule->SetRelativeScale3D(FVector(1.111111f, 1.111111f, 1.111111f));
	PushCapsule->SetCapsuleHalfHeight(33.20842f);
	PushCapsule->SetCapsuleRadius(11.289260f);
	PushCapsule->SetCollisionProfileName(TEXT("Hand"));
}


void AJSH_Player::BeginPlay()
{
	Super::BeginPlay();
	
	// 태어날 때 모든 AX 목록 기억
	FName tag = TEXT("AX");
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AActor::StaticClass() , tag , AXList);

	// // 태어날 때 모든 GassMask 목록 기억
	// FName GMtag = TEXT("GassMask");
	// UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AActor::StaticClass() , GMtag , GMList);

	// 태어날 때 모든 FireEX 목록 기억
	FName Firetag = TEXT("FireEX");
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AActor::StaticClass() , Firetag , FireList);


	if (IsLocallyControlled() && !PlayerMainUI)
	{
		if (PlayerMainUIFactory)
		{
			PlayerMainUI = CreateWidget<UUserWidget>(GetWorld(), PlayerMainUIFactory);
			if(PlayerMainUI)
			{
				PlayerMainUI->AddToViewport(0);
				UE_LOG(LogTemp, Warning, TEXT("Drone UI created for local player"));
			}
		}
	}

	
	// KJHPlayerState = Cast<AKJH_PlayerState>(GetPlayerState());



	// APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	// if (PlayerController)
	// {
	// 	KJHPlayerState = Cast<AKJH_PlayerState>(PlayerController->GetPlayerState());
	// }

	// KJHPlayerState = Cast<AKJH_PlayerState>( GetPlayerState());
}


void AJSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (FireEXSprayOnBool)
	{
		FireEXSprayTrace(DeltaTime);
	}
}


void AJSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(VoiceAction, ETriggerEvent::Started, this, &AJSH_Player::StartVoiceChat);
		input->BindAction(VoiceAction, ETriggerEvent::Completed, this, &AJSH_Player::CancelVoiceChat);
	}

	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJSH_Player::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AJSH_Player::Look);

		// See Watch (Q)
		EnhancedInputComponent->BindAction(WatchAction, ETriggerEvent::Started, this, &AJSH_Player::Watch);

		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AJSH_Player::Grab);

		EnhancedInputComponent->BindAction(ReadyAction, ETriggerEvent::Started, this, &AJSH_Player::R);
		
		// (Left Mouse) - 1) 도끼 찍기, 2) 소화기
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AJSH_Player::LeftMouse);

		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &AJSH_Player::Walk);

		
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Started, this, &AJSH_Player::E);
		
		// EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &AJSH_Player::Slide);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AJSH_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AJSH_Player, WatchSee);
	DOREPLIFETIME(AJSH_Player, AxModeON);
	DOREPLIFETIME(AJSH_Player, AxAnimationplay);
	DOREPLIFETIME(AJSH_Player, WantWalk);
	DOREPLIFETIME(AJSH_Player, WantSprint);
	DOREPLIFETIME(AJSH_Player, BHasAX);
	DOREPLIFETIME(AJSH_Player, PossibleWalk);
	DOREPLIFETIME(AJSH_Player, GassMaskOn);
	DOREPLIFETIME(AJSH_Player, BHasFire);
	DOREPLIFETIME(AJSH_Player, FireEXOn);
	DOREPLIFETIME(AJSH_Player, FireEXSprayOnBool);
	DOREPLIFETIME(AJSH_Player, FlashON);
	DOREPLIFETIME(AJSH_Player, PushMode);
	DOREPLIFETIME(AJSH_Player, PushPossible);
}




void AJSH_Player::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);


		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AJSH_Player::Look(const FInputActionValue& Value)
{

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}



// 시계 UI 보기 ------------------------------------------------------------------
void AJSH_Player::Watch(const FInputActionValue& Value)
{
	Server_WatchSee();
}

void AJSH_Player::Server_WatchSee_Implementation()
{
	NetMulti_WatchSee();
}

void AJSH_Player::NetMulti_WatchSee_Implementation()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp && MeshComp->GetAnimInstance())
	{
		// Flip-Flop
		if (WatchSee)
		{
			MeshComp->GetAnimInstance()->Montage_Play(WatchMontage, 1.0f);
		}
		else
		{
			MeshComp->GetAnimInstance()->Montage_Play(WatchReverseMontage, 1.0f);
		}

		
		WatchSee = !WatchSee;
	}
}
// @----------------------------------------------------------------------------




// 걷기 -------------------------------------

// 대부분 ABP에서 해결, 네트워크 동기화를 위해 Walk <-> Run 바꿔주는 부분만 .. 이것만 동기화가 안 됌 ...
void AJSH_Player::Walk(const FInputActionValue& Value)
{
	Server_Walk();
}

void AJSH_Player::Server_Walk_Implementation()
{
	NetMulti_Walk();
}

void AJSH_Player::NetMulti_Walk_Implementation()
{
	if (AxModeON == false && FireEXOn == false)
	{
		WantWalk = !WantWalk;
	}
}

// ------------------------------------




// * Push Actor =================================================
void AJSH_Player::E(const FInputActionValue& Value)
{
	Server_E();
}




void AJSH_Player::NetMulti_E_Implementation()
{
	NetMulti_E();
}

void AJSH_Player::Server_E_Implementation()
{
	
	if (FireEXOn == false)
	{
		if (PushMode)
		{
			PushMode = false;
			PushPossible = false;
			WantWalk = false;
			//PushCapsule->SetCollisionProfileName(TEXT("NoCollision"));
		}
		else
		{
			PushMode = true;
			PushPossible = true;
			//PushCapsule->SetCollisionProfileName(TEXT("Hand"));
			WantWalk = true;

			MyReleaseFire();
			MyReleaseAX();
			
			AxModeON = false;
			FireEXOn = false;
			FireEXNiagara->Deactivate();
			FireEXSprayOnBool = false;
		
			
			if (WatchSee == false)
			{
				USkeletalMeshComponent* MeshComp = GetMesh();
				MeshComp->GetAnimInstance()->Montage_Play(WatchReverseMontage, 3.0f);
				WatchSee = true;
			}
		}
	}
	
	

}
// ================================================= {Push Actor}





// Flash On / Off
void AJSH_Player::R(const FInputActionValue& Value)
{
	Server_RedyAction();
}



void AJSH_Player::Server_RedyAction_Implementation()
{
	NetMulti_RedyAction();
}

void AJSH_Player::NetMulti_RedyAction_Implementation()
{
	if (FlashON)
	{
		FlashLight->SetVisibility(false);
		FlashON = false;
	}
	else
	{
		FlashLight->SetVisibility(true);
		FlashON = true;
	}
}







void AJSH_Player::Grab(const FInputActionValue& Value)
{
	Server_Grab();
}

void AJSH_Player::Server_Grab_Implementation()
{
	NetMulti_Grab();
}

void AJSH_Player::NetMulti_Grab_Implementation()
{
	//// gengine->AddOnScreenDebugMessage(9, 3, FColor::Green, FString::Printf(TEXT("grab")));
	
	
	if ( BHasAX )
	{
		MyReleaseAX();
		// WantWalk = true;
		AxModeON = false;
		WantWalk = false;
		if (WatchSee == false)
		{
			USkeletalMeshComponent* MeshComp = GetMesh();
			MeshComp->GetAnimInstance()->Montage_Play(WatchReverseMontage, 3.0f);
			WatchSee = true;
		}
	}
	else
	{
		MyTakeAX();
	}

	
	if ( BHasFire )
	{
		MyReleaseFire();
		WantWalk = false;
		FireEXOn = false;
		FireEXNiagara->Deactivate();
		FireEXSprayOnBool = false;
		if (WatchSee == false)
		{
			USkeletalMeshComponent* MeshComp = GetMesh();
			MeshComp->GetAnimInstance()->Montage_Play(WatchReverseMontage, 3.0f);
			WatchSee = true;
		}
	}
	else
	{
		MyTakeFire();
	}
}

void AJSH_Player::MyTakeAX()
{
	// // gengine->AddOnScreenDebugMessage(9, 3, FColor::Green, FString::Printf(TEXT("take")));

	for ( AActor* AX : AXList )
	{
		float tempDist = GetDistanceTo(AX);
		if ( tempDist > GrabDistance )
			continue;
		if ( nullptr != AX->GetOwner() )
			continue;
		
		
		GrabAXActor = AX;
	
		AX->SetOwner(this);
		BHasAX = true;


		AttachAX(GrabAXActor);
		AxModeON = true;
		// WantWalk = true;

		if (!GrabAXActor->ActorHasTag(FName("Count")))
		{
			GrabAXActor->Tags.Add(FName("Count"));

			KJHPlayerState = Cast<AKJH_PlayerState>( GetPlayerState());
			if (KJHPlayerState && KJHPlayerState != nullptr)
			{
				KJHPlayerState->IncrementPersonItemUsedCount();
				// gengine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ccount: %d"), KJHPlayerState->PersonState_ItemUsedCount));
			}
		}


		
		break;
	}
}

void AJSH_Player::MyReleaseAX()
{
	// gengine->AddOnScreenDebugMessage(9, 3, FColor::Green, FString::Printf(TEXT("re")));

	if ( false == BHasAX)
		return;
	


	if ( BHasAX )
	{
		BHasAX = false;
	}


	if ( GrabAXActor )
	{
		DetachAX(GrabAXActor);

		GrabAXActor->SetOwner(nullptr);

		GrabAXActor = nullptr;
	}


	AxModeON = false;
}

void AJSH_Player::AttachAX(AActor* AXActor)
{
	// gengine->AddOnScreenDebugMessage(9, 3, FColor::Green, FString::Printf(TEXT("attach")));
	GrabAXActor = AXActor;
	auto* mesh = GrabAXActor->GetComponentByClass<UStaticMeshComponent>();
	check(mesh);
	if ( mesh )
	{
		mesh->SetSimulatePhysics(false);
		mesh->AttachToComponent(HandComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void AJSH_Player::DetachAX(AActor* AXActor)
{
	// gengine->AddOnScreenDebugMessage(9, 3, FColor::Green, FString::Printf(TEXT("detach")));
	// 도끼의 메쉬를 가져와서
	auto* mesh = AXActor->GetComponentByClass<UStaticMeshComponent>();
	check(mesh);
	if ( mesh )
	{
		// 물리를 켜주고싶다.
		mesh->SetSimulatePhysics(true);
		// 분리하고싶다..
		mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}




void AJSH_Player::MyTakeFire()
{

	for ( AActor* Fire : FireList )
	{

		float tempFireDist = GetDistanceTo(Fire);
		if ( tempFireDist > GrabDistance )
			continue;
		
		if ( nullptr != Fire->GetOwner() )
			continue;
		


		GrabFireActor = Fire;
		
		Fire->SetOwner(this);
		BHasFire = true;

		AttachFire(GrabFireActor);
		break;
	}
}

void AJSH_Player::MyReleaseFire()
{
	// 소화기를 잡고 있지 않다면 버릴 수 없음
	if ( false == BHasFire)
		return;
	

	// 소화기를 이미 잡은 상태 -> 놓고싶다.
	if ( BHasFire )
	{
		BHasFire = false;
	}

	// 소화기의 오너를 취소하고싶다.
	if ( GrabFireActor )
	{
		DetachFire(GrabFireActor);

		GrabFireActor->SetOwner(nullptr);
		// 소화기를 잊고싶다.
		GrabFireActor = nullptr;
	}
	
}



void AJSH_Player::AttachFire(AActor* FireActor)
{
	GrabFireActor = FireActor;
	auto* mesh = GrabFireActor->GetComponentByClass<UStaticMeshComponent>();
	check(mesh);
	if ( mesh )
	{
		mesh->SetSimulatePhysics(false);
		mesh->AttachToComponent(FireHandComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale);


		if (GrabFireActor->ActorHasTag(FName("Count")))
		{
			// gengine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("no")));
		}
		else
		{
			GrabFireActor->Tags.Add(FName("Count"));

			 KJHPlayerState = Cast<AKJH_PlayerState>( GetPlayerState());
			if (KJHPlayerState && KJHPlayerState != nullptr)
			{
				KJHPlayerState->IncrementPersonItemUsedCount();
				// gengine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ccount: %d"), KJHPlayerState->PersonState_ItemUsedCount));
			}
		}
	}

	
}

void AJSH_Player::DetachFire(AActor* FireActor)
{

	auto* mesh = FireActor->GetComponentByClass<UStaticMeshComponent>();
	check(mesh);
	if ( mesh )
	{

		mesh->SetSimulatePhysics(true);
		mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}











// Left Mouse 인터렉션 ------------------------------------------------------------------
void AJSH_Player::LeftMouse(const FInputActionValue& Value)
{
	Server_LeftMouseAction();
}

void AJSH_Player::Server_LeftMouseAction_Implementation()
{
	NetMulti_LeftMouseAction();
}

void AJSH_Player::NetMulti_LeftMouseAction_Implementation()
{
	// // gengine->AddOnScreenDebugMessage(8, 1, FColor::Blue, FString::Printf(TEXT("3")));

	// 도끼를 잡고 있다면
	if (BHasAX)
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp && MeshComp->GetAnimInstance())
		{
			MeshComp->GetAnimInstance()->Montage_Play(AxMontage, 1.0f);
			PlaySoundAtLocation();
		}
	}

	if (BHasFire)
	{
		if (FireEXOn)
		{
			FireEXOn = false;
			WantWalk = false;
			FireEXNiagara->Deactivate();
			FireEXSprayOnBool = false;
		}
		else
		{
			FireEXOn = true;
			WantWalk = true;
			if (WatchSee == false)
			{
				USkeletalMeshComponent* MeshComp = GetMesh();
				MeshComp->GetAnimInstance()->Montage_Play(WatchReverseMontage, 3.0f);
				WatchSee = true;
			}
			FireEXNiagara->Activate();
			FireEXSprayOnBool = true;
			
			// FVector Start = FireEXNiagara->GetComponentLocation();
			// FVector End = FireEXNiagara->GetComponentLocation() + (FireEXNiagara->GetForwardVector() * 500.0f);
			// FHitResult HitResult;
			// FCollisionQueryParams params;
			// params.AddIgnoredActor(this);
			// bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, params);
		}
	}
}


void AJSH_Player::FireEXSprayTrace(float DeltaTime)
{
	currtime += DeltaTime;

	if (spraytime <= currtime)
	{
	
	    FVector StartLocation = FireEXNiagara->GetComponentLocation();
	    
	  
	    FVector ForwardVector = FireEXNiagara->GetForwardVector();
	    

	    FVector EndLocation = StartLocation + (ForwardVector * 450.0f);
	    

	    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	    

	    float SphereRadius = 100.0f;


	    TArray<AActor*> ActorsToIgnore;
	    ActorsToIgnore.Add(this);  


	    TArray<FHitResult> OutHits;


	    bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
	        GetWorld(),
	        StartLocation,
	        EndLocation,
	        SphereRadius,
	        ObjectTypes,
	        false,
	        ActorsToIgnore,
	        EDrawDebugTrace::None, 
	        OutHits,
	        true
	    );
	    
		
	    if (bHit)
	    {
	        for (auto& Hit : OutHits)
	        {

	            AActor* HitActor = Hit.GetActor();
	            
	            if (HitActor && HitActor->ActorHasTag(FName("FireOnOff"))) 
	            {
	                HitActor->Destroy();
	            	
	                break;
	            }
	        }
	    }

		currtime = 0;
	}
}



void AJSH_Player::AllOff()
{
	AxModeON = false;
	
	MyReleaseFire();
	WantWalk = false;
	
	FireEXOn = false;
	FireEXNiagara->Deactivate();
	FireEXSprayOnBool = false;
	

	
	MyReleaseAX();
	AxModeON = false;
	
	if (WatchSee == false)
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		MeshComp->GetAnimInstance()->Montage_Play(WatchReverseMontage, 3.0f);
		WatchSee = true;
	}
}




// ------------------------------------------------------------------------






void AJSH_Player::StartVoiceChat(const FInputActionValue& Value)
{
	GetController<AKJH_PlayerController>()->StartTalking();
}

void AJSH_Player::CancelVoiceChat(const FInputActionValue& Value)
{
	GetController<AKJH_PlayerController>()->StopTalking();
}





void AJSH_Player::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	

	if (OtherActor->ActorHasTag(FName("FRoomPlayer")) && !OtherActor->ActorHasTag(FName("PlayerFinded")))
	{
		KJHPlayerState = Cast<AKJH_PlayerState>( GetPlayerState());

		if (KJHPlayerState && KJHPlayerState != nullptr)
		{
			KJHPlayerState->IncrementPersonSearchRoomCount_Implementation();

			OtherActor->Tags.Add(FName("PlayerFinded"));

			// gengine->AddOnScreenDebugMessage(-3, 5.f, FColor::Red, FString::Printf(TEXT("FindRoomCount: %d"), KJHPlayerState->PersonState_SearchRoomCount));
		}

		
		// Server_Overlap_Room(OtherActor);
	}
    
	if (OtherActor->ActorHasTag(FName("OverlapFire")))
	{
		// Server_Overlap_Fire(OtherActor);

		KJHPlayerState = Cast<AKJH_PlayerState>( GetPlayerState());

		if (KJHPlayerState && KJHPlayerState != nullptr)
		{
			KJHPlayerState->IncrementPersonDamageCount();

			// 왜인지 모르겠는데 client애서 접촉 시 +2씩 카운트가 됌 .... 모르겠따
			KJHPlayerState->PersonState_DamageCount -= 1;

			// gengine->AddOnScreenDebugMessage(-3, 5.f, FColor::Red, FString::Printf(TEXT("DamageCount: %d"), KJHPlayerState->PersonState_DamageCount));
		}
	}
}

void AJSH_Player::PlaySoundAtLocation()
{
	if (SoundToPlay)
	{
		// Get the actor's current location and play the sound at that location
		FVector ActorLocation = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, ActorLocation);
	}
}



