// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioCaptureComponent.h"
#include "Sound/SoundWave.h"
#include "KHS_AudioCaptureComp.generated.h"


#define SYNTHCOMPONENT_EX_OSCILATOR_ENABLED 0


UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class MTVS_3RDUNREAL_API UKHS_AudioCaptureComp : public UAudioCaptureComponent
{
	GENERATED_BODY()
	
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;

	// Called to generate more audio
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	// Sets the oscillator's frequency
	UFUNCTION(BlueprintCallable, Category = "Synth|Components|Audio")
	void SetFrequency(const float FrequencyHz = 440.0f);

protected:
#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
	// A simple oscillator class. Can also generate Saw/Square/Tri/Noise.
	Audio::FOsc Osc;
#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED


public:


	// 음성 녹음을 시작하는 함수
	UFUNCTION(BlueprintCallable, Category = "Audio Capture")
	void StartRecording();

	// 음성 녹음을 종료하고 WAV 파일로 저장하는 함수
	UFUNCTION(BlueprintCallable, Category = "Audio Capture")
	void StopRecordingAndSave();

	// 현재 녹음 중인지 여부
	bool bIsRecording;

	// 녹음된 데이터를 저장할 SoundWave 객체
	class USoundWave* CapturedSoundWave;

	// 녹음 중인 데이터를 저장할 버퍼
	TArray<uint8> AudioBuffer;

	// 녹음된 SoundWave 데이터를 WAV 파일로 저장하는 함수
	void SaveWavFile(const FString& FilePath);


};