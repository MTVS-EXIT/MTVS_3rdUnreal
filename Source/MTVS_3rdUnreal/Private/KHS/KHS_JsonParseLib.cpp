// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_JsonParseLib.h"

TArray<uint8> UKHS_JsonParseLib::JsonParseGetAIImage(const FString& json)
{

	TArray<uint8> arrayData;
	//Json Reader ����
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	//Parsing����� ���� ���� ����
	TSharedPtr<FJsonObject> JsonResult;
	//���޹��� json �ؼ�
	if (FJsonSerializer::Deserialize(JsonReader, JsonResult) && JsonResult.IsValid())
	{
		// "detected_image" �ʵ尡 �����ϴ��� Ȯ��
		if (JsonResult->HasTypedField<EJson::String>(TEXT("detected_image")))
		{
			FString Base64String = JsonResult->GetStringField(TEXT("detected_image"));

			FBase64::Decode(Base64String, arrayData);
			// Base64 ���ڵ�
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
	//ȣ���� �ʿ� ����� Value�� ��ȯ
	return arrayData;
}

FString UKHS_JsonParseLib::MakeJson(const TMap<FString, FString> source)
{
	//source�� JsonObject�������� �����
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	for (TPair<FString, FString> pair : source)
	{
		jsonObject->SetStringField(pair.Key, pair.Value);
	}

	//writer�� ����
	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);

	//JsonObject�� ���ڵ��ϰ�
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);

	//���ڵ������ ��ȯ�Ѵ�.
	return json;
}
