// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_JsonParseLib.h"

TArray<uint8> UKHS_JsonParseLib::JsonParseGetAIImage(const FString& json)
{

	TArray<uint8> arrayData;
	//Json Reader 생성
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	//Parsing결과를 담을 변수 생성
	TSharedPtr<FJsonObject> JsonResult;
	//전달받은 json 해석
	if (FJsonSerializer::Deserialize(JsonReader, JsonResult) && JsonResult.IsValid())
	{
		// "detected_image" 필드가 존재하는지 확인
		if (JsonResult->HasTypedField<EJson::String>(TEXT("detected_image")))
		{
			FString Base64String = JsonResult->GetStringField(TEXT("detected_image"));

			FBase64::Decode(Base64String, arrayData);
			// Base64 디코딩
			if (false == FBase64::Decode(Base64String, arrayData))
			{
				UE_LOG(LogTemp, Error, TEXT("Base64 Decoding Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Can not find detected_image field"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JSON Pasing Failed"));
	}
	//호출한 쪽에 적용된 Value값 반환
	return arrayData;
}

FString UKHS_JsonParseLib::MakeJson(const TMap<FString, FString> source)
{
	//source를 JsonObject형식으로 만들고
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	for (TPair<FString, FString> pair : source)
	{
		jsonObject->SetStringField(pair.Key, pair.Value);
	}

	//writer를 만들어서
	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);

	//JsonObject를 인코딩하고
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);

	//인코딩결과를 반환한다.
	return json;
}
