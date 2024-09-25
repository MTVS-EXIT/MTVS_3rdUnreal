﻿#include "KJH/KJH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "KHS/KHS_DronePlayer.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "KJH/KJH_InGameWidget.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "KHS/KHS_DroneMainUI.h"
#include "JSH_PlayerMainUI.h"

void AKJH_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay에서 ShowCharacterSelectWidget을 호출하여 로컬 클라이언트에서만 UI를 생성하도록 설정
    if (IsLocalController()) // 로컬 클라이언트일 때 실행
    {
        ShowCharacterSelectWidget();
    }

    // InGameWidget 초기화
    if (InGameWidgetFactory)
    {
        InGameWidget = CreateWidget<UKJH_InGameWidget>(this, InGameWidgetFactory);
        if (InGameWidget)
        {
            InGameWidget->SetMyInterface(Cast<IKJH_Interface>(GetGameInstance()));
            bIsInGameWidgetVisible = false; // 처음엔 false로 설정하여 InGameWidget이 안보이게 설정
        }
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
        Subsystem->AddMappingContext(IMC_Common, 1);
}

void AKJH_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);

    if (EnhancedInput)
        EnhancedInput -> BindAction(IA_ToggleInGameWidget, ETriggerEvent::Triggered, this, &AKJH_PlayerController::ToggleInGameWidget);

}

void AKJH_PlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    // 캐릭터가 Possess된 후에도 UI를 표시하도록 설정
    if (IsLocalController()) // 로컬 클라이언트일 때 캐릭터 선택 UI 송출
        ShowCharacterSelectWidget();

    // 서버에서만 Possess 정보를 처리
    if (HasAuthority())
        Server_NotifyPawnPossessed(aPawn);
}

// 사용자 정의형 함수 구간 - 캐릭터 선택 및 스폰 관련 ================================================================================
void AKJH_PlayerController::ShowCharacterSelectWidget()
{
    // 로컬 컨트롤러에서만 UI를 생성하도록 설정
    if (IsLocalPlayerController())
    {
        if (!CharacterSelectWidget && CharacterSelectWidgetFactory)
        {
            // 위젯 생성
            CharacterSelectWidget = CreateWidget<UKJH_CharacterSelectWidget>(this, CharacterSelectWidgetFactory);
            if (CharacterSelectWidget)
            {
                // 위젯 설정 및 뷰포트에 추가
                CharacterSelectWidget->Setup();
                CharacterSelectWidget->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("CharacterSelectWidget Created and Setup for Player."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create CharacterSelectWidget! Check if the Factory is valid."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to create CharacterSelectWidget on a non-local controller."));
    }
}

void AKJH_PlayerController::ClientShowCharacterSelectWidget_Implementation()
{
    ShowCharacterSelectWidget();
}

void AKJH_PlayerController::SpawnCharacterBasedOnSelection()
{
    if (false == HasAuthority()) // 클라이언트에서 실행된 경우
    {
        ServerSpawnCharacterBasedOnSelection(bIsPersonCharacterSelected);
        return;
    }

    // 서버에서의 실행 로직
    TSubclassOf<APawn> ChosenCharacterClass = bIsPersonCharacterSelected
        ? BP_JSH_PlayerClass
        : BP_KHS_DronePlayerClass;

    FVector NewSpawnLocation;
    FRotator NewSpawnRotation;

    if (bIsPersonCharacterSelected)
    {
        if (CharacterSelectWidgetFactory)
        {
            // 위젯 생성
            CharacterSelectWidget = CreateWidget<UKJH_CharacterSelectWidget>(this, CharacterSelectWidgetFactory);
            if (CharacterSelectWidget)
            {
                // 위젯 설정 및 뷰포트에 추가
                CharacterSelectWidget->Setup();
                CharacterSelectWidget->ShowSpawnWidget();
                UE_LOG(LogTemp, Warning, TEXT("CharacterSelectWidget is Setting!"));
            }
        }

        // 사람 스폰 포인트 찾기
        TArray<AActor*> FoundPersonSpawns;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PersonSpawnPoint"), FoundPersonSpawns);

        if (FoundPersonSpawns.Num() > 0)
        {
            AActor* PersonSpawnPoint = FoundPersonSpawns[0]; // 첫번째로 찾은 Person Spawn Point 사용
            NewSpawnLocation = PersonSpawnPoint->GetActorLocation();
            NewSpawnRotation = PersonSpawnPoint->GetActorRotation();
        }
        else
        {
            // 검색에 실패할 경우, 기본 위치로 스폰
            NewSpawnLocation = FVector(0.0f, 0.0f, 200.0f);
            NewSpawnRotation = FRotator::ZeroRotator;
        }
    }
    else // 드론이 선택됐을 경우,
    {

        if (CharacterSelectWidgetFactory)
        {
            // 위젯 생성
            CharacterSelectWidget = CreateWidget<UKJH_CharacterSelectWidget>(this, CharacterSelectWidgetFactory);
            if (CharacterSelectWidget)
            {
                // 위젯 설정 및 뷰포트에 추가
                CharacterSelectWidget->Setup();
                CharacterSelectWidget->ShowSpawnWidget();
                UE_LOG(LogTemp, Warning, TEXT("CharacterSelectWidget is Setting!"));
            }
        }

        // 드론 스폰 포인트 찾기
        TArray<AActor*> FoundDroneSpawns;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DroneSpawnPoint"), FoundDroneSpawns);

        if (FoundDroneSpawns.Num() > 0)
        {
            AActor* DroneSpawnPoint = FoundDroneSpawns[0]; // 첫번째로 찾은 Drone Spawn Point 사용
            NewSpawnLocation = DroneSpawnPoint->GetActorLocation();
            NewSpawnRotation = DroneSpawnPoint->GetActorRotation();
        }
        else
        {
            // 검색에 실패할 경우, 기본 위치로 스폰
            NewSpawnLocation = FVector(0.0f, 0.0f, 100.0f);
            NewSpawnRotation = FRotator::ZeroRotator;
        }
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetPawn();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenCharacterClass, NewSpawnLocation, NewSpawnRotation, SpawnParams);
    if (NewPawn)
    {
        // 네트워크 설정
        NewPawn->SetReplicates(true);
        NewPawn->SetReplicateMovement(true);
        Possess(NewPawn);
        UE_LOG(LogTemp, Warning, TEXT("Character successfully spawned for Player at location: %s"), *NewSpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn character for Player."));
    }
}

void AKJH_PlayerController::ServerSpawnCharacterBasedOnSelection_Implementation(bool bIsPersonSelected)
{
    bIsPersonCharacterSelected = bIsPersonSelected;
    SpawnCharacterBasedOnSelection();
}

bool AKJH_PlayerController::ServerSpawnCharacterBasedOnSelection_Validate(bool bIsPersonSelected)
{
    return true;
}

// 사용자 정의형 함수 구간 - 드론 & 플레이어 고유 UI 생성 관련 ================================================================================
// ServerRPC를 통해 Possess 정보를 확인하고 적절한 UI 설정을 트리거하는 함수
void AKJH_PlayerController::Server_NotifyPawnPossessed_Implementation(APawn* aPawn)
{
    // 서버에서 Pawn 타입을 확인하고 적절한 UI 설정 함수를 호출
    if (aPawn->IsA(AKHS_DronePlayer::StaticClass()))
    {
        Client_SetupDroneUI();
    }
    else if (aPawn->IsA(AJSH_Player::StaticClass()))
    {
        Client_SetupPersonUI();
    }
}

// 드론 UI를 설정하는 ClientRPC
void AKJH_PlayerController::Client_SetupDroneUI_Implementation()
{
    //// 로컬 클라이언트에서만 드론 UI를 생성 및 표시
    //if (DroneWidgetFactory && IsLocalController())
    //{
    //    DroneWidget = CreateWidget<UKHS_DroneMainUI>(this, DroneWidgetFactory);
    //    if (DroneWidget)
    //    {
    //        DroneWidget->AddToViewport();
    //        UE_LOG(LogTemp, Warning, TEXT("Drone UI created for local player"));
    //    }
    //}
}

// 소방관 UI를 설정하는 ClientRPC
void AKJH_PlayerController::Client_SetupPersonUI_Implementation()
{
    // 로컬 클라이언트에서만 소방관 UI를 생성 및 표시
    if (PersonWidgetFactory && IsLocalController())
    {
        PersonWidget = CreateWidget<UJSH_PlayerMainUI>(this, PersonWidgetFactory);
        if (PersonWidget)
        {
            PersonWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("Person UI created for local player"));
        }
    }
}

// 사용자 정의형 함수 구간 - 인게임 UI 관련 ================================================================================
// InGameWidget ON/ OFF 함수
void AKJH_PlayerController::ToggleInGameWidget(const FInputActionValue& Value)
{
    if (false == IsLocalPlayerController())
    {
        return;
    }

    if (nullptr == InGameWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("InGameWidget is not Valid!"));
        return;
    }

    if (bIsInGameWidgetVisible)
    {
        InGameWidget->Teardown();
        bIsInGameWidgetVisible = false;
    }
    else
    {
        InGameWidget->Setup();
        bIsInGameWidgetVisible = true;
    }
}


