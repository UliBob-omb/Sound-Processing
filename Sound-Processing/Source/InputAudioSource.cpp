#include "InputAudioSource.h"

InputAudioSource::InputAudioSource(int channels, juce::AudioIODevice* device)
{
	channelAmount = channels;
	currentDevice = device;

	centerFrequencies cntrFreqs;
	centerFreqs.at(0) = cntrFreqs.HF_Hz;
	centerFreqs.at(1) = cntrFreqs.LF_Hz;
	centerFreqs.at(2) = cntrFreqs.HMF_Hz;
	centerFreqs.at(3) = cntrFreqs.LMF_Hz;

	levelPreLinear.resize(channelAmount);
	levelPostLinear.resize(channelAmount);
	for (int i = 0; i < channelAmount; i++)
	{
		HF_IIR_Filters.add(new juce::IIRFilter);
		ChangeFilter(0, boosts.at(0), QVals.at(0), centerFreqs.at(0));
		LF_IIR_Filters.add(new juce::IIRFilter);
		ChangeFilter(1, boosts.at(1), QVals.at(1), centerFreqs.at(1));
		HMF_IIR_Filters.add(new juce::IIRFilter);
		ChangeFilter(2, boosts.at(2), QVals.at(2), centerFreqs.at(2));
		LMF_IIR_Filters.add(new juce::IIRFilter);
		ChangeFilter(3, boosts.at(3), QVals.at(3), centerFreqs.at(3));
	}
}

InputAudioSource::~InputAudioSource()
{

}

void InputAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;
	if (channelAmount > 0)
	{
		for (int i = 0; i < 4; i++)
		{
			ChangeFilter(i, boosts.at(i), QVals.at(i), centerFreqs.at(i));
		}
	}
}

void InputAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	// Your audio-processing code goes here!

	// For more details, see the help for AudioProcessor::getNextAudioBlock()
	//auto* device = deviceManager.getCurrentAudioDevice();
	auto activeInChnnls = currentDevice->getActiveInputChannels();
	//auto activeOutChnnls = currentDevice->getActiveOutputChannels();
	auto maxInChnnls = activeInChnnls.getHighestBit() + 1;
	//auto maxOutChnnls = activeOutChnnls.getHighestBit() + 1;
	/*if (maxInChnnls != channelAmount)
	{
		channelAmount = maxInChnnls;
	}*/

	// Input Channels
	for (auto channel = 0; channel < maxInChnnls; ++channel)
	{
		float inRMSlvl = bufferToFill.buffer->getRMSLevel(channel, bufferToFill.startSample, bufferToFill.numSamples);
		levelPreLinear.at(channel) = inRMSlvl;
		if (maxInChnnls == 0)
		{
		}
		else
		{
			if (!activeInChnnls[channel] || isMuted == true)
			{
				bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
			}
			else
			{
				auto* inputBuffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
				auto* outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					outputBuffer[sample] = inputBuffer[sample] * float(gain);
				}
				HF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
				LF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
				HMF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
				LMF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
			}
			float inRMSlvl = bufferToFill.buffer->getRMSLevel(channel, bufferToFill.startSample, bufferToFill.numSamples);
			levelPostLinear.at(channel) = inRMSlvl;
		}
	}
}

void InputAudioSource::releaseResources()
{

}

float InputAudioSource::GetLinearLevel(bool isPostProcess)
{
	float level = 0;
	level = isPostProcess ? levelPostLinear.at(0) : levelPreLinear.at(0);
	return level;
}

void InputAudioSource::SetGain(double newGain)
{
	gain = newGain;
}

void InputAudioSource::SetMuted(bool isMuted)
{
	InputAudioSource::isMuted = isMuted;
}

bool InputAudioSource::IsMuted()
{
	return isMuted;
}

void InputAudioSource::SetEnabledEQ(bool isEnabled)
{
	isEQEnabled = isEnabled;
	if (!isEQEnabled)
	{
		for (int channel = 0; channel < channelAmount; channel++)
		{
			HF_IIR_Filters[channel]->makeInactive();
			LF_IIR_Filters[channel]->makeInactive();
			HMF_IIR_Filters[channel]->makeInactive();
			LMF_IIR_Filters[channel]->makeInactive();
		}
	}
	else if (isEnabled)
	{
		for (int channel = 0; channel < channelAmount; channel++)
		{
			HF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makeHighShelf(currentSampleRate, centerFreqs.at(0), QVals.at(0), boosts.at(0)));
			LF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makeLowShelf(currentSampleRate, centerFreqs.at(1), QVals.at(1), boosts.at(1)));
			HMF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreqs.at(2), QVals.at(2), boosts.at(2)));
			LMF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreqs.at(3), QVals.at(3), boosts.at(3)));
			HF_IIR_Filters[channel]->reset();
			LF_IIR_Filters[channel]->reset();
			LMF_IIR_Filters[channel]->reset();
			HMF_IIR_Filters[channel]->reset();
		}
	}
}

bool InputAudioSource::IsEQEnabled()
{
	return isEQEnabled;
}

void InputAudioSource::ChangeFilter(int filterIdx, double gainBoost, double Q, double centerFreq)
{
	/* Used to change one filter using the filter index, with given gain boost, Q value, and center frequency.
	   If resetting all filters after a sample rate change, use <InputAudioSource>.
	*/
	boosts.at(filterIdx) = gainBoost;
	QVals.at(filterIdx) = Q;
	centerFreqs.at(filterIdx) = centerFreq;
	for (int i = 0; i < channelAmount; i++)
	{
		switch (filterIdx) {
		case 0:
			HF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makeHighShelf(currentSampleRate, centerFreqs.at(filterIdx), QVals.at(filterIdx), boosts.at(filterIdx)));
			break;
		case 1:
			LF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makeLowShelf(currentSampleRate, centerFreqs.at(filterIdx), QVals.at(filterIdx), boosts.at(filterIdx)));
			break;
		case 2:
			LF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreqs.at(filterIdx), QVals.at(filterIdx), boosts.at(filterIdx)));
			break;
		case 3:
			LF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreqs.at(filterIdx), QVals.at(filterIdx), boosts.at(filterIdx)));
			break;
		}
	}
}

void InputAudioSource::DeviceUpdateCallback(juce::AudioIODevice* newDevice)
{
	// AudioAppComponents can call this to update each input's device
	currentDevice = newDevice;
}
