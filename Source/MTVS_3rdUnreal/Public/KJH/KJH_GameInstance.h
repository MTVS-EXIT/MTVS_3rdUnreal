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
	void LoadStartMenu(); // 시작화면 UI를 불러오는 함수

	UFUNCTION()
	void RefreshServerList();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu(); // 인게임 UI를 불러오는 함수

	UFUNCTION(BlueprintCallable)
	void LoadServerMenuMap(); // 인게임 UI가 있는 맵으로 로드하는 함수 (UI는 레벨에 붙어있기 때문)


////////// TSubclass & class 참조 구간 -----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerWidgetFactory; // ServerWidget(UI) 공장
	class UKJH_ServerWidget* ServerWidget; // ServerWidget(UI) 선언

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> InGameWidgetFactory; // InGameWidget(UI) 공장
	class UKJH_WidgetSystem* InGameWidget; // InGameWidget(UI) 선언

////////// 전역 변수 & 인스턴스 구간 -----------------------------------------------------------------------------------------

	IOnlineSessionPtr SessionInterface; // 세션 인터페이스를 전역인수로 선언
	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // 온라인 세션 검색을 할 수 있는 클래스 인스턴스 선언
};
