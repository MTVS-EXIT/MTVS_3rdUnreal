// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KHS_JsonParseLib.generated.h"

//class ResponseKey
//{
//public:
//	FString AIImage;
//	FString UserName;
//	FString accessToken;
//};


/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API UKHS_JsonParseLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public: 

	//AI �̹��� ó�� ��ȯ �̺�Ʈ Json Reader �Լ�
	TArray<uint8> JsonParseGetAIImage(const FString& json);

	//AI STT ó�� ��ȯ �̺�Ʈ Json Reader �Լ�
	FString JsonParseGetAIText(const FString& json);

	//AI STS ó�� ��ȯ �̺�Ʈ Json Reader �Լ�
	TArray<uint8> JsonParseGetAIAudio(const FString& json);


	//Json Writer �Լ�
	static FString MakeJson(const TMap<FString, FString> source);

};
