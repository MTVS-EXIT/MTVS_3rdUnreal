// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "KJH_Interface.h"
#include "UObject/Interface.h"
#include "KJH_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKJH_GameInstance : public UGameInstance, public IKJH_Interface
{
	GENERATED_BODY()
	
public:

// GameInstance의 에디터 실행 시 생성자 함수 선언 (초기화만 해줌)
	UKJH_GameInstance(const FObjectInitializer& ObjectInitializer);

// GameInstance의 BeginPlay 시 생성자 함수 선언 (초기화만 해줌)
	virtual void Init() override;



////////// 델리게이트 바인딩 함수 구간 ----------------------------------------------------------------------------
	
	void OnCreateSessionComplete(FName SessionName, bool Success); // 세션 생성 완료 시 호출될 함수
	void OnDestroySessionComplete(FName SessionName, bool Success); // 세션 파괴 완료 시 호출될 함수
	void OnFindSessionComplete(bool Success); // 세션 찾기 완료 시 호출될 함수. Success 인자만 있으면 된다.
											  // 발견된 세션 목록은 SeesionSearch TSharedRef 포인터에 있기 때문이다.

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

////////// 사용자 정의형 함수 구간 --------------------------------------------------------------------------------
	UFUNCTION(Exec) // Exec: 콘솔창에 입력할 수 있도록 만든다.
	void Host(); // 서버 열기 함수

	UFUNCTION(Exec)
	void Join(uint32 Index); // 서버 접속 함수

	UFUNCTION()
	void CreateSession(); // 세션을 만드는 함수

	UFUNCTION(BlueprintCallable)
	void LoadServerWidget(); // 시작화면 UI를 불러오는 함수

	UFUNCTION()
	void RefreshServerList();

	UFUNCTION(BlueprintCallable, Category = "Load Widget")
	void LoadInGameWidget(); // 인게임 UI를 불러오는 함수

	UFUNCTION(BlueprintCallable, Category = "Load Widget")
	void LoadServerWidgetMap(); // 인게임 UI가 있는 맵으로 로드하는 함수 (UI는 레벨에 붙어있기 때문)

	// 캐릭터 선택 관련 함수 //
	UFUNCTION(BlueprintCallable, Category = "Character Selection")
	void OnCharacterSelected(APlayerController* PlayerController, bool bIsSelectedPersonFromUI); // 플레이어가 선택한 캐릭터에 따라 컨트롤러가 선택되는 함수

    UFUNCTION(Server, Reliable, WithValidation) // ServerRPC 는 클라이언트가 서버에게 요청하는 것이므로, Validate와 Implementation 으로 나눠서 구성
    void ServerNotifyCharacterSelected(APlayerController* PlayerController, bool bIsSelectedPerson); // 서버에서 클라이언트의 캐릭터 선택을 처리하는 RPC 함수 선언
	bool ServerNotifyCharacterSelected_Validate(APlayerController* PlayerController, bool bIsSelectedPerson); // 서버 RPC 함수의 유효성 검사 (이 정보를 보내도 되는지, 아닌지)
	void ServerNotifyCharacterSelected_Implementation(APlayerController* PlayerController, bool bIsSelectedPerson); // 서버 RPC 함수의 구현부 (요청이 승인되면, 실제론 여기서 실행)

////////// TSubclass & class 참조 구간 -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ServerWidgetFactory; // ServerWidget(UI) 공장
	class UKJH_ServerWidget* ServerWidget; // ServerWidget(UI) 참조 선언

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> InGameWidgetFactory; // InGameWidget(UI) 공장
	class UKJH_WidgetSystem* InGameWidget; // InGameWidget(UI) 참조 선언

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UKJH_CharacterSelectWidget> CharacterSelectWidgetFactory; // CharacterSelectWidget(UI) 공장
	class UKJH_CharacterSelectWidget* CharacterSelectWidget; // CharacterSelectWidget(UI) 참조 선언

////////// 전역 변수 & 인스턴스 구간 -----------------------------------------------------------------------------------------

	IOnlineSessionPtr SessionInterface; // 세션 인터페이스를 전역인수로 선언
	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // 온라인 세션 검색을 할 수 있는 클래스 인스턴스 선언

	// 캐릭터 선택 상태 관리
	bool bIsPersonSelected = false; // UI 상에서 사람이 선택되었는지 체크
	bool bIsDroneSelected = false;  // UI 상에서 드론이 선택되었는지 체크

////////// 타이머핸들 선언 구간 -----------------------------------------------------------------------------------------
};
