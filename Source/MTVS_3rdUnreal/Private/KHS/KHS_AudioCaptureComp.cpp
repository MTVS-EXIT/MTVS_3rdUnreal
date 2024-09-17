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

	// �ʱ� ���´� ���� ���� �ƴ�
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
	//	// AudioCaptureComponent�� �ʱ�ȭ�ϰ� ���� ����
	//	Start(); // ����� ĸó ����

	//	// ���� �ʱ�ȭ
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
	//	// AudioCaptureComponent�� ������ ����
	//	Stop(); // ����� ĸó ����
	//	bIsRecording = false;

	//	// ������ ����� �����͸� ���ۿ� ����
	//	GetAudioBufferData(AudioBuffer);

	//	// ���۸� SoundWave ��ü�� ��ȯ
	//	CapturedSoundWave->RawData.Lock(LOCK_READ_WRITE);
	//	void* LockedData = CapturedSoundWave->RawData.Realloc(AudioBuffer.Num());
	//	FMemory::Memcpy(LockedData, AudioBuffer.GetData(), AudioBuffer.Num());
	//	CapturedSoundWave->RawData.Unlock();

	//	// SoundWave ����
	//	CapturedSoundWave->NumChannels = 1; // ��� ä��
	//	CapturedSoundWave->Duration = AudioBuffer.Num() / (2 * 44100.0f); // 16��Ʈ, 44.1kHz ����
	//	CapturedSoundWave->SetSampleRate(44100); // ���� ����Ʈ ����

	//	// WAV ���Ϸ� ������ ��� ����
	//	FString FilePath = FPaths::ProjectDir() + TEXT("RecordedAudio.wav");
	//	SaveWavFile(FilePath);

	//	UE_LOG(LogTemp, Log, TEXT("���� ���� �� ���� ����: %s"), *FilePath);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("������ ���� ������ �ʽ��ϴ�."));
	//}
}

void UKHS_AudioCaptureComp::SaveWavFile(const FString& FilePath)
{
	//// SoundWave �����͸� WAV ���Ϸ� ��ȯ �� ����
	//if (CapturedSoundWave)
	//{
	//	// SoundWave �����͸� ���Ϸ� ����
	//	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*FilePath);

	//	if (FileWriter)
	//	{
	//		// RawData�� ����� ������ ����� ������ ��������
	//		const void* LockedData = CapturedSoundWave->RawData.LockReadOnly();
	//		FileWriter->Serialize(const_cast<void*>(LockedData), CapturedSoundWave->RawData.GetBulkDataSize());
	//		CapturedSoundWave->RawData.Unlock();

	//		// ���� ���� �Ϸ�
	//		FileWriter->Close();
	//		delete FileWriter;

	//		UE_LOG(LogTemp, Log, TEXT("WAV ���� ���� �Ϸ�: %s"), *FilePath);
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("WAV ���� ���� ����: %s"), *FilePath);
	//	}
	//}
}
