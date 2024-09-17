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


	// ���� ������ �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Audio Capture")
	void StartRecording();

	// ���� ������ �����ϰ� WAV ���Ϸ� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Audio Capture")
	void StopRecordingAndSave();

	// ���� ���� ������ ����
	bool bIsRecording;

	// ������ �����͸� ������ SoundWave ��ü
	class USoundWave* CapturedSoundWave;

	// ���� ���� �����͸� ������ ����
	TArray<uint8> AudioBuffer;

	// ������ SoundWave �����͸� WAV ���Ϸ� �����ϴ� �Լ�
	void SaveWavFile(const FString& FilePath);


};