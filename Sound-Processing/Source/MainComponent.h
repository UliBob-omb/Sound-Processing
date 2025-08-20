#pragma once

#include <JuceHeader.h>
#include <vector>
#include "LevelMeter.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //=================================Audio Thread=================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //=============================Handler Functions================================
    void sliderValueChanged(juce::Slider* slider) override;

    void buttonClicked(juce::Button* button) override;
    //==============================================================================
    // Your private member variables go here...
    juce::Label mstrFdrLabel;
    juce::Slider masterFader;
    double masterGain = 0.0;

    LevelMeter mstrLevelMeterL = LevelMeter([&]() -> float {return juce::Decibels::gainToDecibels(outputLevelsLinear.at(0)); }, true);
    LevelMeter mstrLevelMeterR = LevelMeter([&]() -> float {return juce::Decibels::gainToDecibels(outputLevelsLinear.at(1)); }, true);
    std::vector<float> outputLevelsLinear;

    //============Per-Channel Components==============

    int createdChannels = 0;
    
    juce::OwnedArray<LevelMeter> inLevelMeters;
    std::vector<float> inputLevelsLinear;

    juce::OwnedArray<juce::Slider> channelFaders;
    std::vector<double> channelGains;

    juce::OwnedArray<juce::TextButton> FocusButtons;
    juce::OwnedArray<juce::TextButton> MuteButtons;
    juce::OwnedArray<juce::TextButton> ListenButtons;
    int soloChannel = -1;

    juce::OwnedArray<juce::Slider> HFBoosts;
    juce::OwnedArray<juce::Slider> LFBoosts;

    juce::OwnedArray<juce::Slider> HMFBoosts;
    juce::OwnedArray<juce::Slider> HMFQLevels;
    juce::OwnedArray<juce::Slider> HMFCenterFreq;

    juce::OwnedArray<juce::Slider> LMFBoosts;
    juce::OwnedArray<juce::Slider> LMFQLevels;
    juce::OwnedArray<juce::Slider> LMFCenterFreq;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
