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
		UE_LOG(LogTemp, Error, TEXT("JSON PARSING FAILED"));
	}
	//ȣ���� �ʿ� ����� Value�� ��ȯ
	return arrayData;
}

//AI STT ó�� ��ȯ �̺�Ʈ Json Reader �Լ�
FString UKHS_JsonParseLib::JsonParseGetAIText(const FString& json)
{
	FString OutputText;
	//Json Reader ����
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	//Parsing����� ���� ���� ����
	TSharedPtr<FJsonObject> JsonObject;
	//���޹��� json �ؼ�
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// "output_text" �ʵ尡 �����ϴ��� Ȯ��
		if (JsonObject->HasField(TEXT("output_text")))
		{
			//�ش� �ʵ��� ���� OutputText�� ���
			OutputText = JsonObject->GetStringField(TEXT("output_text"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Can not find output_text field on Json Data"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JSON PARSING FAILED"));
	}

	return OutputText;
}

//AI STS ó�� ��ȯ �̺�Ʈ Json Reader �Լ�
TArray<uint8> UKHS_JsonParseLib::JsonParseGetAIAudio(const FString& json)
{
	TArray<uint8> AudioData;
	//Json Reader ����
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	// Parsing����� ���� ���� ����
	TSharedPtr<FJsonObject> JsonObject;
	//���޹��� json �ؼ�
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// "output_audio" �ʵ尡 �����ϴ��� Ȯ��
		if (JsonObject->HasField(TEXT("output_audio")))
		{
			//�ش� String �ʵ��� ���� Audio Data�� ��ȯ
			FString Base64String = JsonObject->GetStringField(TEXT("output_audio"));
			// Base64 ���ڵ�
			FBase64::Decode(Base64String, AudioData);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Can not find output_audio field on Json Data"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JSON PARSING FAILED"));
	}

	return AudioData;
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
