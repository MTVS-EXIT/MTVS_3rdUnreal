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
		UE_LOG(LogTemp, Error, TEXT("JSON PARSING FAILED"));
	}
	//호출한 쪽에 적용된 Value값 반환
	return arrayData;
}

//AI STT 처리 반환 이벤트 Json Reader 함수
FString UKHS_JsonParseLib::JsonParseGetAIText(const FString& json)
{
	FString OutputText;
	//Json Reader 생성
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	//Parsing결과를 담을 변수 생성
	TSharedPtr<FJsonObject> JsonObject;
	//전달받은 json 해석
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// "output_text" 필드가 존재하는지 확인
		if (JsonObject->HasField(TEXT("output_text")))
		{
			//해당 필드의 값을 OutputText에 담기
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

//AI STS 처리 반환 이벤트 Json Reader 함수
TArray<uint8> UKHS_JsonParseLib::JsonParseGetAIAudio(const FString& json)
{
	TArray<uint8> AudioData;
	//Json Reader 생성
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	// Parsing결과를 담을 변수 생성
	TSharedPtr<FJsonObject> JsonObject;
	//전달받은 json 해석
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// "output_audio" 필드가 존재하는지 확인
		if (JsonObject->HasField(TEXT("output_audio")))
		{
			//해당 String 필드의 값을 Audio Data로 변환
			FString Base64String = JsonObject->GetStringField(TEXT("output_audio"));
			// Base64 디코딩
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

// AI봇에 감지된 태그들 추출 이벤트 Json Reader 함수
TArray<FString> UKHS_JsonParseLib::JsonParseGetDetectedTags(const FString& json)
{
	TArray<FString> DetectedTags;

	// Json Reader 생성
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	TSharedPtr<FJsonObject> JsonResult;

	if (FJsonSerializer::Deserialize(JsonReader, JsonResult) && JsonResult.IsValid())
	{
		// "categories" 필드가 존재하는지 확인
		if (JsonResult->HasField(TEXT("categories")))
		{
			TSharedPtr<FJsonObject> CategoriesObject = JsonResult->GetObjectField(TEXT("categories"));

			// "caution", "danger", "safe" 필드 확인
			for (const auto& Category : { TEXT("caution"), TEXT("danger"), TEXT("safe") })
			{
				if (CategoriesObject->HasTypedField<EJson::Object>(Category))
				{
					TSharedPtr<FJsonObject> CategoryObject = CategoriesObject->GetObjectField(Category);
					int32 DetectedValue = CategoryObject->GetIntegerField(TEXT("detected"));

					// "detected"가 1인 경우 해당 태그를 추가
					if (DetectedValue == 1)
					{
						DetectedTags.Add(Category);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON for detected tags"));
	}

	return DetectedTags;
}

// AI봇에 감지된 물품들 Count추출 이벤트 Json Reader 함수
TArray<int32> UKHS_JsonParseLib::JsonParseGetDetectedCount(const FString& json)
{
	TArray<int32> DetectedCounts = { 0, 0, 0 }; // 초기화 (safe: 0, caution: 1, danger: 2)

	// Json Reader 생성
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(json);
	TSharedPtr<FJsonObject> JsonResult;

	if (FJsonSerializer::Deserialize(JsonReader, JsonResult) && JsonResult.IsValid())
	{
		// "categories" 필드가 존재하는지 확인
		if (JsonResult->HasField(TEXT("categories")))
		{
			TSharedPtr<FJsonObject> CategoriesObject = JsonResult->GetObjectField(TEXT("categories"));

			// "caution", "danger", "safe" 필드 확인 및 count 값 추출
			for (int32 Index = 0; Index < 3; ++Index)
			{
				FString CategoryName = (Index == 0) ? TEXT("safe") : (Index == 1) ? TEXT("caution") : TEXT("danger");
				if (CategoriesObject->HasTypedField<EJson::Object>(CategoryName))
				{
					TSharedPtr<FJsonObject> CategoryObject = CategoriesObject->GetObjectField(CategoryName);
					int32 CountValue = CategoryObject->GetIntegerField(TEXT("count"));
					DetectedCounts[Index] = CountValue; // 해당 카테고리의 count 값을 저장
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON for detected counts"));
	}

	// 로그로 DetectedCounts 배열 출력
	for (int32 i = 0; i < DetectedCounts.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("Category %d Count: %d"), i, DetectedCounts[i]);
	}

	return DetectedCounts; // count 배열 반환
}

//Json Writer 함수
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
