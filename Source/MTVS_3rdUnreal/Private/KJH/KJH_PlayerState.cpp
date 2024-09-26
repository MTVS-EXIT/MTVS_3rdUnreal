// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "HttpModule.h"
#include "KJH/KJH_GameInstance.h"
#include "HttpFwd.h"
#include "KJH/KJH_ResultWidget.h"
#include "Engine/World.h"


AKJH_PlayerState::AKJH_PlayerState()
{
    //// 초기값 설정
    //bIsGameEnded = false; // 게임이 시작되었을 때는 종료되지 않은 상태로 설정
}

////////// 사용자 정의형 함수 구간 - 네트워크 복제 설정 함수 ----------------------------------------------------------------------------------------------------------------
// 네트워크 복제 설정 함수
void AKJH_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsPersonCharacterSelected 변수가 서버에서 클라이언트로 복제되도록 설정
    DOREPLIFETIME(AKJH_PlayerState, bIsPersonCharacterSelected);

    //DOREPLIFETIME(AKJH_PlayerState, bIsGameEnded);

    DOREPLIFETIME(AKJH_PlayerState, PersonState_SearchRoomCount);
    DOREPLIFETIME(AKJH_PlayerState, PersonState_ItemUsedCount);
    DOREPLIFETIME(AKJH_PlayerState, PersonState_DamageCount);

    DOREPLIFETIME(AKJH_PlayerState, DroneState_DetectedCount);
    DOREPLIFETIME(AKJH_PlayerState, DroneState_DetectedCautionCount);
    DOREPLIFETIME(AKJH_PlayerState, DroneState_DetectedDangerCount);
    DOREPLIFETIME(AKJH_PlayerState, DroneState_DetectedSafeCount);
}

////////// 사용자 정의형 함수 구간 - 소방관 이벤트 시 카운트 함수 ===============================================================================
void AKJH_PlayerState::IncrementPersonSearchRoomCount_Implementation()
{
    PersonState_SearchRoomCount++;
}

void AKJH_PlayerState::IncrementPersonItemUsedCount_Implementation()
{
    PersonState_ItemUsedCount++;
}

void AKJH_PlayerState::IncrementPersonDamageCount_Implementation()
{
    PersonState_DamageCount++;
}

////////// 사용자 정의형 함수 구간 - 드론 이벤트 시 카운트 함수 ===============================================================================
// 탐지 시도 횟수 카운트
void AKJH_PlayerState::IncrementDroneDetectedCount_Implementation()
{
    DroneState_DetectedCount ++; // 횟수 더하기
}

// 탐지 결과 카테고리 별 카운트
void AKJH_PlayerState::IncrementDroneCategoryCount_Implementation(int32 CategoryNum, int32 Num)
{
    switch (CategoryNum)
    {
    case 0: // Safe 의 경우,
        DroneState_DetectedSafeCount += Num; // 갯수를 인자로 받아 더하기
        break;
    case 1: // Caution 의 경우,
        DroneState_DetectedCautionCount += Num; // 갯수를 인자로 받아 더하기
        break;
    case 2: // Danger 경우,
        DroneState_DetectedDangerCount += Num; // 갯수를 인자로 받아 더하기
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid category number for drone detection"));
        return;
    }
}

////////// 사용자 정의형 함수 구간 - 백엔드 팀에게 전달하는 정보 관련 함수 ===============================================================================

void AKJH_PlayerState::SendDataToServer()
{
    // 1) JSON 객체 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // 2) PlayerState의 모든 관련 데이터를 JSON 객체에 추가
    JsonObject->SetBoolField("bIsPersonCharacterSelected", bIsPersonCharacterSelected);  // 플레이어가 소방관인지 드론인지 여부



    JsonObject->SetNumberField("PersonState_SearchRoomCount", PersonState_SearchRoomCount);  // 소방관이 탐색한 방의 수
    JsonObject->SetNumberField("PersonState_ItemUsedCount", PersonState_ItemUsedCount);  // 소방관이 사용한 아이템의 수
    JsonObject->SetNumberField("PersonState_DamageCount", PersonState_DamageCount);  // 소방관이 받은 데미지 횟수
    JsonObject->SetNumberField("DroneState_DetectedCount", DroneState_DetectedCount);  // 드론이 감지한 총 횟수
    JsonObject->SetNumberField("DroneState_DetectedSafeCount", DroneState_DetectedSafeCount);  // 드론이 감지한 안전 물체의 수
    JsonObject->SetNumberField("DroneState_DetectedCautionCount", DroneState_DetectedCautionCount);  // 드론이 감지한 주의 물체의 수
    JsonObject->SetNumberField("DroneState_DetectedDangerCount", DroneState_DetectedDangerCount);  // 드론이 감지한 위험 물체의 수

    // 3) JSON 객체를 문자열로 변환
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // 4) HTTP 요청 객체 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    // 5) 요청 완료 시 호출될 함수 설정
    Request->OnProcessRequestComplete().BindUObject(this, &AKJH_PlayerState::OnDataSendComplete);

    // 6) 요청을 보낼 서버의 URL 설정
    Request->SetURL("http://125.132.216.190:7757/api/result/save"); // 백엔드 URL 삽입

    // 7) HTTP 메소드를 POST로 설정
    Request->SetVerb("POST");

    // 8) 요청 헤더에 콘텐츠 타입을 JSON으로 설정
    Request->SetHeader("Content-Type", "application/json");

    // GameInstance에서 AuthToken 가져오기
    if (UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance()))
    {
        FString AuthToken = GameInstance->GetAuthToken();
        if (false == AuthToken.IsEmpty())
        {
            Request->SetHeader("Authorization", AuthToken);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Auth token is empty. Request may fail."));
        }
    }

    // 요청 본문에 JSON 문자열 설정
    Request->SetContentAsString(JsonString);

    // 설정된 HTTP 요청을 서버로 전송
    Request->ProcessRequest();
}

void AKJH_PlayerState::OnDataSendComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Data sent successfully to server."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to send data to server."));
    }
}

////////// Temp ======================================================================
void AKJH_PlayerState::Multicast_TriggerGameEnd_Implementation()
{
    // 서버에서 실행 중인 경우 (리슨 서버 포함)
    if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
    {
        // 모든 플레이어에 대해 ShowResultWidget 호출
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC)
            {
                AKJH_PlayerState* PS = PC->GetPlayerState<AKJH_PlayerState>();
                if (PS)
                {
                    PS->ShowResultWidget();
                }
            }
        }
    }

    // 리슨 서버의 로컬 플레이어이거나 일반 클라이언트인 경우
    if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_Client)
    {
        ShowResultWidget();
    }
}

void AKJH_PlayerState::ShowResultWidget()
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (PC && ResultWidgetClass)
    {
        UKJH_ResultWidget* ResultWidget = CreateWidget<UKJH_ResultWidget>(PC, ResultWidgetClass);
        if (ResultWidget)
        {

            UKJH_GameInstance* GameInstance = Cast<UKJH_GameInstance>(GetWorld()->GetGameInstance());
            if (GameInstance)
            {
                ResultWidget->SetMyInterface(GameInstance);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance"));
            }
            ResultWidget->Setup();
            ResultWidget->SetResultData(PersonState_SearchRoomCount, PersonState_ItemUsedCount, PersonState_DamageCount,
                                        DroneState_DetectedCount, DroneState_DetectedSafeCount, DroneState_DetectedCautionCount, DroneState_DetectedDangerCount);
            ResultWidget->PlayResultAnimations();
        }
    }
}
