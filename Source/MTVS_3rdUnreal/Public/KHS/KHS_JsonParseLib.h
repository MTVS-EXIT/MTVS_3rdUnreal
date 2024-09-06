// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KHS_JsonParseLib.generated.h"

class ResponseKey
{
public:
	FString AIImage;
	FString UserName;
	FString accessToken;
};


/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKHS_JsonParseLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public: 

	//AI 이미지 처리 반환 이벤트 Json Reader 함수
	TArray<uint8> JsonParseGetAIImage(const FString& json);

	static FString MakeJson(const TMap<FString, FString> source);

};
