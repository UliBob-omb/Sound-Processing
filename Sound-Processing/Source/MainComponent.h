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

    //==============================GUI Audio Thread=================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //=====================================GUI=======================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //=============================Handler Functions================================
    void sliderValueChanged(juce::Slider* slider) override;

    void buttonClicked(juce::Button* button) override;

    void updateAllFilters();

    void toggleFilter(int channel);

    //==============================================================================
    // Your private member variables go here...
    juce::Label mstrFdrLabel;
    juce::Slider masterFader;
    double masterGain = 0.0;
    double currentSampleRate = 48000.;
    int curSamplesPerBlockExpected = 480;

    // Default GUI Dimensions/Padding
    const int INNER_PAD = 10;
    const int OUTER_PAD = 15;
    const float MASTER_HEIGHT_PRCNT = 0.20f;
    const float MASTER_WIDTH_PRCNT = 1.00f;
    const float CHANNEL_HEIGHT_PRCNT = 0.80f;
    const int CHANNEL_WIDTH = 180;
    const int MASTER_LVL_MTR_WIDTH = 15;
    const int LABEL_HEIGHT = 20;
    const int FADER_WIDTH = 20;
    const int BIG_DIAL_WIDTH = 65;
    const int SMALL_DIAL_WIDTH = 55;
    const int CHANNEL_LVL_MTR_WIDTH = 20;
    const int CHANNEL_BTN_WIDTH = 80;
    const int CHANNEL_BTN_HEIGHT = 30;

    // Default Channel Values
    const double defaultdBFS = -10.0;
    const double defaultMidPointdBFS = -18.0;
    const double defaultQ = .707;
    const double defaultBoostGain = 1.;
    const double defaultFreqHMF = 5338.;
    const double defaultFreqLMF = 450.;
    const double defaultCutoffHF = 18000.;
    const double defaultCutoffLF = 50.;

    LevelMeter mstrLevelMeterL = LevelMeter([&]() -> float {return juce::Decibels::gainToDecibels(outputLevelsLinear.at(0)); }, true);
    LevelMeter mstrLevelMeterR = LevelMeter([&]() -> float {return juce::Decibels::gainToDecibels(outputLevelsLinear.at(1)); }, true);
    std::vector<float> outputLevelsLinear;

    //============Per-Channel Components==============

    int createdChannels = 0;
    
    juce::OwnedArray<LevelMeter> inLevelMeters;
    std::vector<float> inputLevelsLinear;

    juce::OwnedArray<juce::Slider> channelFaders;
    std::vector<double> channelGains;

    juce::OwnedArray<juce::TextButton> EQToggleButtons;
    std::vector<bool> enabledChannelEQs;

    juce::OwnedArray<juce::TextButton> MuteButtons;
    std::vector<bool> mutedChannels;

    juce::OwnedArray<juce::TextButton> ListenButtons;
    int soloChannel = -1;

    //juce::dsp::IIR::Filter<float> newFilter;
    juce::OwnedArray<juce::Label> HFLabels;
    juce::OwnedArray<juce::IIRFilter> HF_IIR_Filters;
    juce::OwnedArray<juce::Slider> HFBoosts;
    juce::OwnedArray<juce::Slider> HFQLevels;
    juce::OwnedArray<juce::Slider> HFCenterFreq;

    juce::OwnedArray<juce::Label> LFLabels;
    juce::OwnedArray<juce::IIRFilter> LF_IIR_Filters;
    juce::OwnedArray<juce::Slider> LFBoosts;
    juce::OwnedArray<juce::Slider> LFQLevels;
    juce::OwnedArray<juce::Slider> LFCenterFreq;

    juce::OwnedArray<juce::Label> HMFLabels;
    juce::OwnedArray<juce::IIRFilter> HMF_IIR_Filters;
    juce::OwnedArray<juce::Slider> HMFBoosts;
    juce::OwnedArray<juce::Slider> HMFQLevels;
    juce::OwnedArray<juce::Slider> HMFCenterFreq;

    juce::OwnedArray<juce::Label> LMFLabels;
    juce::OwnedArray<juce::IIRFilter> LMF_IIR_Filters;
    juce::OwnedArray<juce::Slider> LMFBoosts;
    juce::OwnedArray<juce::Slider> LMFQLevels;
    juce::OwnedArray<juce::Slider> LMFCenterFreq;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
