// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_DroneGameMode.h"

void AKHS_DroneGameMode::BeginPlay()
{
	Super::BeginPlay();

	/*capture = cv::VideoCapture(0);
	if (false == capture.isOpened())
	{
		UE_LOG(LogTemp, Warning, TEXT("Open Webcam Link Failed"));
		return;
	}
	{
		UE_LOG(LogTemp, Warning, TEXT("Open Webcam Link Success"));
	}*/
}
//
////블루프린트로 프레임 읽어내는 함수
//void AKHS_DroneGameMode::ReadScreenFrame()
//{
//	////기존 웹캠 읽어오던 내용은 주석처리
//	////
//	//////비디오캡쳐가 안열려있으면 return
//	////if(false == capture.isOpened())
//	////	return;
//	//////비디오캡쳐에서 매터리얼 읽어내기
//	////capture.read(image);
//
//	////내 스크린 화면 읽어오기 함수 호출
//	//cv::Mat desktopImage = GetScreenToCVMat();
//
//	////매터리얼을 Texture로 변경
//	////imageTexture = MatToTexture2D(image); //기존 웹캠 읽어올때
//	//imageTexture = MatToTexture2D(desktopImage); //스크린화면 읽어올때
//}
//
////내 화면을 CV Mat타입으로 만드는 함수
//cv::Mat AKHS_DroneGameMode::GetScreenToCVMat()
//{
//	////Windows Device Context를 Bitmap형식으로 변환 후 CV Mat형식으로 변환하자.
//
//	////Windows Device Context값 가져오기
//	//HDC hScreenDC = GetDC(NULL);
//	//HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
//	//int screenWidth = GetDeviceCaps(hScreenDC, HORZRES);
//	//int screenHeight = GetDeviceCaps(hScreenDC, VERTRES);
//
//	////Windows Device Context -> Bitmap타입으로 변환
//	//HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
//	//HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
//	//BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY);
//	//SelectObject(hMemoryDC, hOldBitmap);
//
//	////Bitmap타입 -> CV Mat타입 변환
//	//cv::Mat matImage(screenHeight, screenWidth, CV_8UC4);
//	//GetBitmapBits(hBitmap, matImage.total() * matImage.elemSize(), matImage.data);
//
//	////여기서 만든 CV::Mat타입은 CV_8UC4.
//	//return matImage;
//}
//
//
////매터리얼을 텍스쳐로 만드는 함수
//UTexture2D* AKHS_DroneGameMode::MatToTexture2D(const cv::Mat InMat)
//{
//	////새로운 텍스쳐를 생성
//	//UTexture2D* Texture = UTexture2D::CreateTransient(InMat.cols, InMat.rows, PF_B8G8R8A8);
//	//
//	////원하는 타입으로 매터리얼을 조정 후 RETURN.
//	////CV_8UC3타입인 경우(웹캠 스트리밍)
//	//if (InMat.type() == CV_8UC3)//example for pre-conversion of Mat
//	//{
//	//	//매터리얼이 BGR 타입아라면 BGRA 타입 매터리얼로 Convert.(언리얼은 RGBA 4개 채널이니까)
//	//	cv::Mat bgraImage;
//	//	cv::cvtColor(InMat, bgraImage, cv::COLOR_BGR2BGRA);
//
//	//	//Texture->SRGB = 0;//set to 0 if Mat is not in srgb 
//	//	// (which is likely when coming from a webcam)
//	//	//(선택) 다른 세팅을 하고싶다면 여기에 추가하기.
//	//	//Texture->UpdateResource();
//
//	//	//이미지 데이터를 텍스쳐에 담기
//	//	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
//	//	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);//lock the texture data
//	//	FMemory::Memcpy(Data, bgraImage.data, bgraImage.total() * bgraImage.elemSize());//copy the data
//	//	Mip.BulkData.Unlock();
//	//	Texture->PostEditChange();
//	//	Texture->UpdateResource();
//
//	//	return Texture;
//	//}
//	////CV_8UC4타입인 경우(스크린 화면공유)
//	//else if (InMat.type() == CV_8UC4)
//	//{
//	//	////이미지 데이터를 텍스쳐에 담기
//	//	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
//	//	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);//lock the texture data
//	//	FMemory::Memcpy(Data, InMat.data, InMat.total() * InMat.elemSize());//copy the data
//	//	Mip.BulkData.Unlock();
//	//	Texture->PostEditChange();
//	//	Texture->UpdateResource();
//
//	//	return Texture;
//	//}
//
//	////만약 텍스쳐가 제대로된 픽셀데이터가 없다면 Abort.
//	//Texture->PostEditChange();
//	//Texture->UpdateResource();
//
//	//return Texture;
//}



