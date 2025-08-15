#pragma once

#include <JuceHeader.h>

class LevelMeter : public juce::Component, public juce::Timer
{
public:

	LevelMeter(std::function<float()>&& valueFunction, bool isVertical);

	void resized() override;

	void paint(juce::Graphics& g) override;

	void timerCallback() override;

	//LevelMeter operator =;

private:
	std::function<float()> valueSupplier;
	juce::ColourGradient gradient;
	bool isVertical = true;
};