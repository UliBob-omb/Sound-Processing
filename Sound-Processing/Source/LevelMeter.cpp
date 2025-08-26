#include "LevelMeter.h"

LevelMeter::LevelMeter(std::function<float()>&& valueFunction, bool isVertical) : valueSupplier(std::move(valueFunction))
{
	this->isVertical = isVertical;
	startTimerHz(24);
}

inline void LevelMeter::resized()
{
	const auto bounds = getLocalBounds().toFloat();
	gradient = juce::ColourGradient{ juce::Colours::green, bounds.getBottomLeft(), juce::Colours::red, bounds.getTopLeft(), false };
	gradient.addColour(0.82, juce::Colours::yellow); // starts at ~-18 dBFS if range is -100 to 0dBFS
}

inline void LevelMeter::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds().toFloat();

	g.setColour(juce::Colours::black);
	g.fillRect(bounds);

	g.setGradientFill(gradient);
	if (isVertical)
	{
		const auto scaledY = juce::jmap(valueSupplier(), -100.f, 0.f, 0.f, static_cast<float>(getHeight()));
		g.fillRect(bounds.removeFromBottom(scaledY));
	}
	else
	{
		const auto scaledX = juce::jmap(valueSupplier(), -100.f, 0.f, 0.f, static_cast<float>(getWidth()));
		g.fillRect(bounds.removeFromLeft(scaledX));
	}
}

inline void LevelMeter::timerCallback()
{
	repaint();
}
