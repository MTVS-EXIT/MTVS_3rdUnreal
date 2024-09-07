// Fill out your copyright notice in the Description page of Project Settings.


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
	// Third -> First Camera Feel (1)
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Third -> First Camera Feel (2)
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 


	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	
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
	DigitalWatch->SetupAttachment(GetMesh(), FName("lowerarm_twist_01_l"));
	DigitalWatch->SetRelativeLocation(FVector(2.801333f, -0.27828f, 0.180322f));
	DigitalWatch->SetRelativeRotation(FRotator(0.592057f, 3.560301f, -74.855599f));
	DigitalWatch->SetRelativeScale3D(FVector(1.25f, 1.25f, 1.25f));


	WatchWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WatchWidget"));
	WatchWidget->SetupAttachment(DigitalWatch);
	WatchWidget->SetRelativeLocationAndRotation(FVector(-0.06378f, 2.781286f, 0.11564f), FRotator(0.f, 90.0f, 0.f));
	WatchWidget->SetRelativeScale3D(FVector(0.0013f, 0.003f, 0.003f));

	AX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AX"));
	AX->SetupAttachment(GetMesh(), FName("AX"));
	AX->SetRelativeLocation(FVector(-80.147944f, -916.095325f, 95.000000f));
	AX->SetRelativeRotation(FRotator(0.f, 5.000001f, 0.f));
	AX->SetVisibility(false);
	

}


void AJSH_Player::BeginPlay()
{
	Super::BeginPlay();
	
}


void AJSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AJSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


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
		
		// (Left Mouse) - 1) 도끼 찍기, 2) 소화기
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AJSH_Player::LeftMouse);

		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &AJSH_Player::Walk);
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
	DOREPLIFETIME(AJSH_Player, BhasAX);
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
	WantWalk = !WantWalk;
}

// ------------------------------------






void AJSH_Player::Grab(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(9, 3, FColor::Green, FString::Printf(TEXT("grab")));

	if (BhasAX)
	{
		BhasAX = false;
	}
	else
	{
		BhasAX = true;
	}
}






// Left Mouse 인터렉션 ------------------------------------------------------------------
void AJSH_Player::LeftMouse(const FInputActionValue& Value)
{
	Server_RAction();
	GEngine->AddOnScreenDebugMessage(8, 1, FColor::Blue, FString::Printf(TEXT("1")));
}

void AJSH_Player::Server_RAction_Implementation()
{
	NetMulti_RAction();
}

void AJSH_Player::NetMulti_RAction_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(8, 1, FColor::Blue, FString::Printf(TEXT("3")));
	// AxMode가 ON일때만 도끼 찍는 애니메이션 실행, 추후 도끼 주웠을때 AXMODE가 ON, 도끼 버렸을때 OFF 되도록 하기
	

	// 도끼를 잡고 있다면
	if (AxModeON)
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp && MeshComp->GetAnimInstance())
		{
			MeshComp->GetAnimInstance()->Montage_Play(AxMontage, 1.0f);
			AX->SetVisibility(true);
		}
	}
}
// ------------------------------------------------------------------------