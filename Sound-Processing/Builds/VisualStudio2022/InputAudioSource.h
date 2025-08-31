#pragma once
#include <JuceHeader.h>

class InputAudioSource : public juce::AudioSource
{
public:
	InputAudioSource();
	~InputAudioSource();

	void PrepareToPlay(int samplesPerBLockExpected, double sample) override;
	void GetNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void ReleaseResources() override;

	void GetLinearLevel();
	void SetGain();
	void SetMuted();
	bool IsMuted();
	void SetEnabledEQ();
	bool IsEQEanabled();

	void ChangeFilter(int filterIdx, double gainBoost, double Q, double centerFreq);

private:
	double gain = 1.0;
	bool isMuted = false;
	bool isEQDisabled = false;

	std::vector<double> boosts = std::vector<double>(2, 1.0);
	std::vector<double> QVals = std::vector<double>(2, 0.707);
	std::vector<double> centerFreqs = std::vector<double>(2);
	juce::OwnedArray<juce::IIRFilter> HF_IIR_Filters;
	juce::OwnedArray<juce::IIRFilter> LF_IIR_Filters;
	juce::OwnedArray<juce::IIRFilter> HMF_IIR_Filters;
	juce::OwnedArray<juce::IIRFilter> LMF_IIR_Filters;
};
