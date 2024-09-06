// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_JsonParseLib.h"

TArray<uint8> UKHS_JsonParseLib::JsonParseGetAIImage(const FString& json)
{
	TArray<uint8> arrayData;
	ResponseKey result;
	//Json Reader ����
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);

	//Parsing����� ���� ���� ����
	TSharedPtr<FJsonObject> JsonResult = MakeShareable(new FJsonObject());

	//���޹��� json �ؼ�
	FString ReturnValue;

	if (FJsonSerializer::Deserialize(JsonReader, JsonResult))
	{
		TSharedPtr<FJsonObject> jsonData = JsonResult->GetObjectField(TEXT("file"));

		//�����͸���Ʈ �˻��ؼ� ��Ƶα�
		if (jsonData->HasField("detected_image")) {
			result.AIImage = jsonData->GetStringField("detected_image");	//detected_image �����.
			//Base64 Decoding << Return string
			// TArray<uint8> -> TCHAR
			
			FBase64::Decode(result.AIImage, arrayData);
		}

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
