// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PreOpenCVheaders.h"
#include "PostOpenCVHeaders.h"
#include <opencv2/opencv.hpp>
#include "Engine/Texture2D.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KHS_DroneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_3RDUNREAL_API AKHS_DroneGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	virtual void BeginPlay() override;

	//openCV활용 웹캠 비디오 캡쳐
	cv::VideoCapture capture;
	//openCV활용 매터리얼 이미지
	cv::Mat image;

	//블루프린트로 프레임 읽어내는 함수
	UFUNCTION(BlueprintCallable)
	void ReadScreenFrame();

	//이미지 매터리얼 텍스쳐로 만들기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* imageTexture;

	//매터리얼을 텍스쳐로 만드는 함수
	UTexture2D* MatToTexture2D(const cv::Mat InMat);

	
};
