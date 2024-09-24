// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/KJH_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "HttpModule.h"


////////// 사용자 정의형 함수 구간 - 네트워크 복제 설정 함수 ----------------------------------------------------------------------------------------------------------------
// 네트워크 복제 설정 함수
void AKJH_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsPersonCharacterSelected 변수가 서버에서 클라이언트로 복제되도록 설정
    DOREPLIFETIME(AKJH_PlayerState, bIsPersonCharacterSelected);

    DOREPLIFETIME(AKJH_PlayerState, bIsGameEnded);

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

    // HTTP 요청 객체 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    // 요청 완료 시 호출될 함수 설정
    Request->OnProcessRequestComplete().BindUObject(this, &AKJH_PlayerState::OnDataSendComplete);

    // 요청을 보낼 서버의 URL 설정
    Request->SetURL(""); // 백엔드 URL로 변경 필요

    // HTTP 메소드를 POST로 설정
    Request->SetVerb("POST");

    // 요청 헤더에 콘텐츠 타입을 JSON으로 설정
    Request->SetHeader("Content-Type", "application/json");

    // 요청 본문에 JSON 문자열 설정
    Request->SetContentAsString(JsonString);

    // 설정된 HTTP 요청을 서버로 전송
    Request->ProcessRequest();

}

void AKJH_PlayerState::OnDataSendComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

}
