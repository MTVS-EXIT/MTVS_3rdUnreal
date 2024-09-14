// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DroneGameMode.h"

void AKHS_DroneGameMode::BeginPlay()
{
	Super::BeginPlay();

	capture = cv::VideoCapture(0);
	if (false == capture.isOpened())
	{
		UE_LOG(LogTemp, Warning, TEXT("Open Webcam Link Failed"));
		return;
	}
	{
		UE_LOG(LogTemp, Warning, TEXT("Open Webcam Link Success"));
	}
}

void AKHS_DroneGameMode::ReadScreenFrame()
{
	//����ĸ�İ� �ȿ��������� return
	if(false == capture.isOpened())
		return;
	//����ĸ�Ŀ��� ���͸��� �о��
	capture.read(image);
	//���͸����� Texture�� ����
	imageTexture = MatToTexture2D(image);
}

UTexture2D* AKHS_DroneGameMode::MatToTexture2D(const cv::Mat InMat)
{
	//���ο� �ؽ��ĸ� ����
	UTexture2D* Texture = UTexture2D::CreateTransient(InMat.cols, InMat.rows, PF_B8G8R8A8);
	//���ϴ� Ÿ������ ���͸����� ���� �� RETURN.
	if (InMat.type() == CV_8UC3)//example for pre-conversion of Mat
	{
		//���͸����� BGR Ÿ�Ծƶ�� BGRA Ÿ�� ���͸���� Convert.(�𸮾��� RGBA 4�� ä���̴ϱ�)
		cv::Mat bgraImage;
		cv::cvtColor(InMat, bgraImage, cv::COLOR_BGR2BGRA);

		//Texture->SRGB = 0;//set to 0 if Mat is not in srgb 
		// (which is likely when coming from a webcam)
		//(����) �ٸ� ������ �ϰ�ʹٸ� ���⿡ �߰��ϱ�.
		//Texture->UpdateResource();

		//�̹��� �����͸� �ؽ��Ŀ� ���
		FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);//lock the texture data
		FMemory::Memcpy(Data, bgraImage.data, bgraImage.total() * bgraImage.elemSize());//copy the data
		Mip.BulkData.Unlock();
		Texture->PostEditChange();
		Texture->UpdateResource();

		return Texture;
	}
	//���� �ؽ��İ� ����ε� �ȼ������Ͱ� ���ٸ� Abort.
	Texture->PostEditChange();
	Texture->UpdateResource();

	return Texture;
}
