// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/KHS_AudioCaptureComp.h"
#include "Sound/SoundWave.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/BufferArchive.h"
#include "AudioCaptureCore.h"

bool UKHS_AudioCaptureComp::Init(int32& SampleRate)
{
	NumChannels = 1;

#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
	// Initialize the DSP objects
	Osc.Init(SampleRate);
	Osc.SetFrequency(440.0f);
	Osc.Start();
#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED

	// 초기 상태는 녹음 중이 아님
	bIsRecording = false;
	CapturedSoundWave = NewObject<USoundWave>();


	return true;
}

int32 UKHS_AudioCaptureComp::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
	// Perform DSP operations here
	for (int32 Sample = 0; Sample < NumSamples; ++Sample)
	{
		OutAudio[Sample] = Osc.Generate();
	}
#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED

	return Super::OnGenerateAudio(OutAudio, NumSamples);
}

void UKHS_AudioCaptureComp::SetFrequency(const float InFrequencyHz)
{
#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
	// Use this protected base class method to push a lambda function which will safely execute in the audio render thread.
	SynthCommand([this, InFrequencyHz]()
	{
		Osc.SetFrequency(InFrequencyHz);
		Osc.Update();
	});
#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
}


void UKHS_AudioCaptureComp::StartRecording()
{
	//if (!bIsRecording)
	//{
	//	// AudioCaptureComponent를 초기화하고 녹음 시작
	//	Start(); // 오디오 캡처 시작

	//	// 버퍼 초기화
	//	AudioBuffer.Empty();
	//	bIsRecording = true;

	//	UE_LOG(LogTemp, Log, TEXT("Start Recording"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Is Already Recording....."));
	//}
}

void UKHS_AudioCaptureComp::StopRecordingAndSave()
{
	//if (bIsRecording)
	//{
	//	// AudioCaptureComponent의 녹음을 중지
	//	Stop(); // 오디오 캡처 중지
	//	bIsRecording = false;

	//	// 녹음된 오디오 데이터를 버퍼에 저장
	//	GetAudioBufferData(AudioBuffer);

	//	// 버퍼를 SoundWave 객체로 변환
	//	CapturedSoundWave->RawData.Lock(LOCK_READ_WRITE);
	//	void* LockedData = CapturedSoundWave->RawData.Realloc(AudioBuffer.Num());
	//	FMemory::Memcpy(LockedData, AudioBuffer.GetData(), AudioBuffer.Num());
	//	CapturedSoundWave->RawData.Unlock();

	//	// SoundWave 설정
	//	CapturedSoundWave->NumChannels = 1; // 모노 채널
	//	CapturedSoundWave->Duration = AudioBuffer.Num() / (2 * 44100.0f); // 16비트, 44.1kHz 기준
	//	CapturedSoundWave->SetSampleRate(44100); // 샘플 레이트 설정

	//	// WAV 파일로 저장할 경로 설정
	//	FString FilePath = FPaths::ProjectDir() + TEXT("RecordedAudio.wav");
	//	SaveWavFile(FilePath);

	//	UE_LOG(LogTemp, Log, TEXT("녹음 중지 및 파일 저장: %s"), *FilePath);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("녹음이 진행 중이지 않습니다."));
	//}
}

void UKHS_AudioCaptureComp::SaveWavFile(const FString& FilePath)
{
	//// SoundWave 데이터를 WAV 파일로 변환 및 저장
	//if (CapturedSoundWave)
	//{
	//	// SoundWave 데이터를 파일로 저장
	//	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*FilePath);

	//	if (FileWriter)
	//	{
	//		// RawData에 저장된 녹음된 오디오 데이터 가져오기
	//		const void* LockedData = CapturedSoundWave->RawData.LockReadOnly();
	//		FileWriter->Serialize(const_cast<void*>(LockedData), CapturedSoundWave->RawData.GetBulkDataSize());
	//		CapturedSoundWave->RawData.Unlock();

	//		// 파일 쓰기 완료
	//		FileWriter->Close();
	//		delete FileWriter;

	//		UE_LOG(LogTemp, Log, TEXT("WAV 파일 저장 완료: %s"), *FilePath);
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("WAV 파일 저장 실패: %s"), *FilePath);
	//	}
	//}
}
