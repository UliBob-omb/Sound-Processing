#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::Label mstrFdrLabel;
    juce::Slider masterFader;

    LevelMeter mstrLevelMeterL = LevelMeter([&]() -> float {return juce::Decibels::gainToDecibels(outputLevelsDb.at(0)); }, true);
    LevelMeter mstrLevelMeterR = LevelMeter([&]() -> float {return juce::Decibels::gainToDecibels(outputLevelsDb.at(1)); }, true);
    std::vector<float> outputLevelsDb;
    //============Per-Channel Components==============
    int createdChannels = 0;
    
    juce::OwnedArray<LevelMeter> inLevelMeters;
    std::vector<float> inputLevelsDb;

    juce::OwnedArray<juce::Slider> channelFaders;
    juce::OwnedArray<juce::TextButton> FocusButtons;
    juce::OwnedArray<juce::TextButton> MuteButtons;
    juce::OwnedArray<juce::TextButton> ListenButtons;

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
