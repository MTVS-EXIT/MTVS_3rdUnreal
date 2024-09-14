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

	//openCVȰ�� ��ķ ���� ĸ��
	cv::VideoCapture capture;
	//openCVȰ�� ���͸��� �̹���
	cv::Mat image;

	//�������Ʈ�� ������ �о�� �Լ�
	UFUNCTION(BlueprintCallable)
	void ReadScreenFrame();

	//�̹��� ���͸��� �ؽ��ķ� �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* imageTexture;

	//���͸����� �ؽ��ķ� ����� �Լ�
	UTexture2D* MatToTexture2D(const cv::Mat InMat);

	
};
