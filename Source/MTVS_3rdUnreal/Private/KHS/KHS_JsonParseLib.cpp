// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_JsonParseLib.h"

TArray<uint8> UKHS_JsonParseLib::JsonParseGetAIImage(const FString& json)
{
	TArray<uint8> arrayData;
	ResponseKey result;
	//Json Reader 생성
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);

	//Parsing결과를 담을 변수 생성
	TSharedPtr<FJsonObject> JsonResult = MakeShareable(new FJsonObject());

	//전달받은 json 해석
	FString ReturnValue;

	if (FJsonSerializer::Deserialize(JsonReader, JsonResult))
	{
		TSharedPtr<FJsonObject> jsonData = JsonResult->GetObjectField(TEXT("file"));

		//데이터리스트 검사해서 담아두기
		if (jsonData->HasField("detected_image")) {
			result.AIImage = jsonData->GetStringField("detected_image");	//detected_image 결과값.
			//Base64 Decoding << Return string
			// TArray<uint8> -> TCHAR
			
			FBase64::Decode(result.AIImage, arrayData);
		}

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
