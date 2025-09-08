#pragma once
#include <JuceHeader.h>

class InputAudioSource : public juce::AudioSource
{
public:
	InputAudioSource(int channels, juce::AudioIODevice* device);
	~InputAudioSource();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	float GetLinearLevel(bool isPostProcess);
	void SetGain(double newGain);
	void SetMuted(bool isMuted);
	bool IsMuted();
	void SetEnabledEQ(bool isEnabled);
	bool IsEQEnabled();

	void ChangeFilter(int filterIdx, double gainBoost, double Q, double centerFreq);
	void DeviceUpdateCallback(juce::AudioIODevice* newDevice);
private:
	//======Per InputAudioSource======
	juce::AudioIODevice* currentDevice = nullptr;
	int channelAmount = 0;
	double gain = 1.0;
	bool isMuted = false;
	bool isEQEnabled = true;
	double currentSampleRate = 48000;
	//======Per Filter Set======
	std::vector<double> boosts = std::vector<double>(4, 1.0);
	std::vector<double> QVals = std::vector<double>(4, 0.707);
	std::vector<double> centerFreqs = std::vector<double>(4);
	//======Per Channel======
	std::vector<float> levelPostLinear;
	std::vector<float> levelPreLinear;
	juce::OwnedArray<juce::IIRFilter> HF_IIR_Filters;
	juce::OwnedArray<juce::IIRFilter> LF_IIR_Filters;
	juce::OwnedArray<juce::IIRFilter> HMF_IIR_Filters;
	juce::OwnedArray<juce::IIRFilter> LMF_IIR_Filters;
};

struct centerFrequencies
{
	double HF_Hz = 18000.0;
	double LF_Hz = 50.0;
	double HMF_Hz = 5338.0;
	double LMF_Hz = 450.0;
};
