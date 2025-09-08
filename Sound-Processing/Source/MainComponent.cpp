#include "MainComponent.h"
#include "LevelMeter.h"

//==============================================================================
MainComponent::MainComponent()
{
	// Make sure you set the size of the component after
	// you add any child components.
	int minWinWidth = 800;
	int minWinHeight = 750;
	int inChannelAmt = 2;
	int inputSourceAmt = 1;

	outputLevelsLinear.resize(2);// one stereo output only (for now)
	masterGain = juce::Decibels::decibelsToGain(defaultdBFS);
	//inputLevelsLinear.resize(inChannelAmt);
	//channelGains.resize(inChannelAmt);
	//mutedChannels.resize(inChannelAmt);
	//enabledChannelEQs.resize(inChannelAmt);

	mstrFdrLabel.setEditable(false);
	mstrFdrLabel.setFocusContainerType(juce::Component::FocusContainerType::none);
	mstrFdrLabel.setInterceptsMouseClicks(false, false);
	mstrFdrLabel.setText("Master", juce::dontSendNotification);
	addAndMakeVisible(&mstrFdrLabel);
	
	masterFader.setSliderStyle(juce::Slider::LinearHorizontal);
	masterFader.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
	masterFader.setRange(-100.0, 0.0, 0.05); // -144 dBFS is for 24-bit depth, 16-bit is -96 dBFS
	masterFader.setSkewFactorFromMidPoint(defaultMidPointdBFS);
	masterFader.setPopupDisplayEnabled(true, false, this);
	masterFader.setTextValueSuffix(" dB");
	masterFader.setValue(defaultdBFS);
	masterFader.addListener(this);

	addAndMakeVisible(&masterFader);

	addAndMakeVisible(&mstrLevelMeterL);
	addAndMakeVisible(&mstrLevelMeterR);

	for (int inputIdx = 0; inputIdx < inputSourceAmt; inputIdx++, createdInputs++)
	{
		inputSources.add(new InputAudioSource(inChannelAmt, deviceManager.getCurrentAudioDevice()));
		juce::Slider* fader = new juce::Slider();
		inputFaders.add(fader);
		inputFaders[inputIdx]->setSliderStyle(juce::Slider::LinearVertical);
		inputFaders[inputIdx]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		inputFaders[inputIdx]->setRange(-100.0, 0.0, 0.05);
		inputFaders[inputIdx]->setSkewFactorFromMidPoint(defaultMidPointdBFS);
		inputFaders[inputIdx]->setPopupDisplayEnabled(true, false, this);
		inputFaders[inputIdx]->setTextValueSuffix(" dB");
		inputFaders[inputIdx]->setValue(defaultdBFS);
		inputFaders[inputIdx]->addListener(this);
		addAndMakeVisible(inputFaders[inputIdx]);

		//juce::IIRFilter* filterHF = new juce::IIRFilter();
		//HF_IIR_Filters.add(filterHF);

		juce::Label* labelHF = new juce::Label("HFLabel" + inputIdx, "HF");
		HFLabels.add(labelHF);
		HFLabels[inputIdx]->setJustificationType(juce::Justification::centredTop);
		addAndMakeVisible(HFLabels[inputIdx]);

		juce::Slider* dial1 = new juce::Slider();
		HFBoosts.add(dial1);
		HFBoosts[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HFBoosts[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, BIG_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		HFBoosts[inputIdx]->setRange(-15.0, 15.0, 0.05);
		HFBoosts[inputIdx]->setPopupDisplayEnabled(true, false, this);
		HFBoosts[inputIdx]->setTextValueSuffix(" dB");
		HFBoosts[inputIdx]->setValue(0.0);
		HFBoosts[inputIdx]->addListener(this);
		addAndMakeVisible(HFBoosts[inputIdx]);

		//juce::IIRFilter* filterLF = new juce::IIRFilter;
		//LF_IIR_Filters.add(filterLF);

		juce::Label* labelLF = new juce::Label("LFLabel" + inputIdx, "LF");
		LFLabels.add(labelLF);
		LFLabels[inputIdx]->setJustificationType(juce::Justification::centredBottom);
		addAndMakeVisible(LFLabels[inputIdx]);

		juce::Slider* dial2 = new juce::Slider();
		LFBoosts.add(dial2);
		LFBoosts[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LFBoosts[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxAbove, true, BIG_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		LFBoosts[inputIdx]->setRange(-15.0, 15.0, 0.05);
		LFBoosts[inputIdx]->setPopupDisplayEnabled(true, false, this);
		LFBoosts[inputIdx]->setTextValueSuffix(" dB");
		LFBoosts[inputIdx]->setValue(0.0);
		LFBoosts[inputIdx]->addListener(this);
		addAndMakeVisible(LFBoosts[inputIdx]);

		juce::TextButton* focusButton = new juce::TextButton();
		EQToggleButtons.add(focusButton);
		inputSources[inputIdx]->SetEnabledEQ(true);//enabledChannelEQs.at(channel) = true;
		EQToggleButtons[inputIdx]->setToggleState(inputSources[inputIdx]->IsEQEnabled(), juce::dontSendNotification);
		EQToggleButtons[inputIdx]->setButtonText("EQ");
		EQToggleButtons[inputIdx]->setInterceptsMouseClicks(true, false);
		EQToggleButtons[inputIdx]->setState(juce::Button::buttonNormal);
		EQToggleButtons[inputIdx]->addListener(this);
		addAndMakeVisible(EQToggleButtons[inputIdx]);

		juce::TextButton* muteButton = new juce::TextButton();
		MuteButtons.add(muteButton);
		inputSources[inputIdx]->SetMuted(false);//mutedChannels.at(channel) = false;
		MuteButtons[inputIdx]->setToggleState(inputSources[inputIdx]->IsMuted(), juce::dontSendNotification);
		MuteButtons[inputIdx]->setButtonText("Mute");
		MuteButtons[inputIdx]->setInterceptsMouseClicks(true, false);
		MuteButtons[inputIdx]->setState(juce::Button::buttonNormal);
		MuteButtons[inputIdx]->addListener(this);
		addAndMakeVisible(MuteButtons[inputIdx]);

		juce::TextButton* listenButton = new juce::TextButton();
		ListenButtons.add(listenButton);
		ListenButtons[inputIdx]->setButtonText("Solo");
		ListenButtons[inputIdx]->setInterceptsMouseClicks(true, false);
		ListenButtons[inputIdx]->setState(juce::Button::buttonNormal);
		ListenButtons[inputIdx]->addListener(this);
		addAndMakeVisible(ListenButtons[inputIdx]);

		//juce::IIRFilter* filterHMF = new juce::IIRFilter();
		//HMF_IIR_Filters.add(filterHMF);

		juce::Label* labelHMF = new juce::Label("HMFLabel" + inputIdx, "HMF");
		HMFLabels.add(labelHMF);
		HMFLabels[inputIdx]->setJustificationType(juce::Justification::centredBottom);
		addAndMakeVisible(HMFLabels[inputIdx]);

		juce::Slider* dial3 = new juce::Slider();
		HMFBoosts.add(dial3);
		HMFBoosts[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HMFBoosts[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, BIG_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		HMFBoosts[inputIdx]->setRange(-15.0, 15.0, 0.05);
		HMFBoosts[inputIdx]->setPopupDisplayEnabled(true, false, this);
		HMFBoosts[inputIdx]->setTextValueSuffix(" dB");
		HMFBoosts[inputIdx]->setValue(0.0);
		HMFBoosts[inputIdx]->addListener(this);
		addAndMakeVisible(HMFBoosts[inputIdx]);

		juce::Slider* dial4 = new juce::Slider();
		HMFQLevels.add(dial4);
		HMFQLevels[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HMFQLevels[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, SMALL_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		HMFQLevels[inputIdx]->setRange(0.01, 10.0, 0.01);
		HMFQLevels[inputIdx]->setPopupDisplayEnabled(true, false, this);
		HMFQLevels[inputIdx]->setTextValueSuffix(" Q");
		HMFQLevels[inputIdx]->setValue(2.0);
		HMFQLevels[inputIdx]->addListener(this);
		addAndMakeVisible(HMFQLevels[inputIdx]);

		juce::Slider* dial5 = new juce::Slider();
		HMFCenterFreq.add(dial5);
		HMFCenterFreq[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HMFCenterFreq[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, SMALL_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		HMFCenterFreq[inputIdx]->setRange(00.450, 20.000, 0.001); // y = 4887.5 * x^2 + 450, x = (0, 2)
		HMFCenterFreq[inputIdx]->setPopupDisplayEnabled(true, false, this);
		HMFCenterFreq[inputIdx]->setTextValueSuffix(" kHz");
		HMFCenterFreq[inputIdx]->setSkewFactorFromMidPoint(defaultFreqHMF / 1000);
		HMFCenterFreq[inputIdx]->setValue(defaultFreqHMF / 1000.);
		HMFCenterFreq[inputIdx]->addListener(this);
		addAndMakeVisible(HMFCenterFreq[inputIdx]);

		//juce::IIRFilter* filterLMF = new juce::IIRFilter();
		//LMF_IIR_Filters.add(filterLMF);

		juce::Label* labelLMF = new juce::Label("LMFLabel" + inputIdx, "LMF");
		LMFLabels.add(labelLMF);
		LMFLabels[inputIdx]->setJustificationType(juce::Justification::centredBottom);
		addAndMakeVisible(LMFLabels[inputIdx]);

		juce::Slider* dial6 = new juce::Slider();
		LMFBoosts.add(dial6);
		LMFBoosts[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LMFBoosts[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, BIG_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		LMFBoosts[inputIdx]->setRange(-15.0, 15.0, 0.05);
		LMFBoosts[inputIdx]->setPopupDisplayEnabled(true, false, this);
		LMFBoosts[inputIdx]->setTextValueSuffix(" dB");
		LMFBoosts[inputIdx]->setValue(0.0);
		LMFBoosts[inputIdx]->addListener(this);
		addAndMakeVisible(LMFBoosts[inputIdx]);

		juce::Slider* dial7 = new juce::Slider();
		LMFQLevels.add(dial7);
		LMFQLevels[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LMFQLevels[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, SMALL_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		LMFQLevels[inputIdx]->setRange(0.01, 10.0, 0.01);
		LMFQLevels[inputIdx]->setPopupDisplayEnabled(true, false, this);
		LMFQLevels[inputIdx]->setTextValueSuffix(" Q");
		LMFQLevels[inputIdx]->setValue(2.0);
		LMFQLevels[inputIdx]->addListener(this);
		addAndMakeVisible(LMFQLevels[inputIdx]);

		juce::Slider* dial8 = new juce::Slider();
		LMFCenterFreq.add(dial8);
		LMFCenterFreq[inputIdx]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LMFCenterFreq[inputIdx]->setTextBoxStyle(juce::Slider::TextBoxBelow, true, SMALL_DIAL_WIDTH * 0.8f, LABEL_HEIGHT);
		LMFCenterFreq[inputIdx]->setRange(50.0, 1500.0, 0.001); // y = 362.5 * x^2 + 50, x = (0, 2)
		LMFCenterFreq[inputIdx]->setPopupDisplayEnabled(true, false, this);
		LMFCenterFreq[inputIdx]->setTextValueSuffix(" Hz");
		LMFCenterFreq[inputIdx]->setSkewFactorFromMidPoint(450.0);
		LMFCenterFreq[inputIdx]->setValue(450.0);
		LMFCenterFreq[inputIdx]->addListener(this);
		addAndMakeVisible(LMFCenterFreq[inputIdx]);

		LevelMeter* inLevelMeter = new LevelMeter([inputIdx, this]() -> float {return juce::Decibels::gainToDecibels(inputSources[inputIdx]->GetLinearLevel(true)); }, true);
		inLevelMeters.add(inLevelMeter);
		addAndMakeVisible(inLevelMeters[inputIdx]);
	}

	setSize(minWinWidth, minWinHeight);

	// Some platforms require permissions to open input channels so request that here
	if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
		&& ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
	{
		juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
										   [&] (bool granted) { setAudioChannels (granted ? inChannelAmt : 0, 2); });
	}
	else
	{
		// Specify the number of input and output channels that we want to open
		setAudioChannels (inChannelAmt, 2);
	}
}

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
	// This function will be called when the audio device is started, or when
	// its settings (i.e. sample rate, block size, etc) are changed.

	// You can use this function to initialise any resources you might need,
	// but be careful - it will be called on the audio thread, not the GUI thread.

	// For more details, see the help for AudioProcessor::prepareToPlay()
	currentSampleRate = sampleRate;
	curSamplesPerBlockExpected = samplesPerBlockExpected;

	if (createdInputs > 0)
	{
		// filters are updated automatically in each input source via prepareToPlay callback
		for (int inputIdx = 0; inputIdx < inputSources.size(); inputIdx++)
		{
			inputSources[inputIdx]->DeviceUpdateCallback(deviceManager.getCurrentAudioDevice()); // Might change to per device rather than current device
			inputSources[inputIdx]->prepareToPlay(curSamplesPerBlockExpected, currentSampleRate);
		}
	}
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
	// Your audio-processing code goes here!

	// For more details, see the help for AudioProcessor::getNextAudioBlock()
	auto* device = deviceManager.getCurrentAudioDevice();
	auto activeInChnnls = device->getActiveInputChannels();
	auto activeOutChnnls = device->getActiveOutputChannels();
	auto maxInChnnls = activeInChnnls.getHighestBit() + 1;
	auto maxOutChnnls = activeOutChnnls.getHighestBit() + 1;

	// Pull from the Mixer
	//mixer.getNextAudioBlock(bufferToFill);

	// Input Sources
	/*for (auto channel = 0; channel < maxInChnnls; ++channel)
	{
		if (maxInChnnls == 0)
		{
		}
		else
		{
			if (!activeInChnnls[channel] || mutedChannels[channel] == true)
			{
				bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
			}
			else
			{
				auto* inputBuffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
				auto* outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					outputBuffer[sample] = inputBuffer[sample] * float(channelGains[channel]);
				}
				HF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
				LF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
				HMF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
				LMF_IIR_Filters.getUnchecked(channel)->processSamples(outputBuffer, bufferToFill.numSamples);
			}
			float inRMSlvl = bufferToFill.buffer->getRMSLevel(channel, bufferToFill.startSample, bufferToFill.numSamples);
			inputLevelsLinear.at(channel) = inRMSlvl;
		}
	}*/
	for (auto input = 0; input < inputSources.size(); input++)
	{
		inputSources[input]->getNextAudioBlock(bufferToFill);
	}
	//Master Output
	for (auto channel = 0; channel < maxOutChnnls; ++channel)
	{
		if ((!activeOutChnnls[channel]) || maxInChnnls == 0)
		{
			bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
		}
		else if (soloChannel >= 0) // Change to if statement once master output and listen output are decoupled
		{
			if (!activeInChnnls[soloChannel])
			{
				soloChannel = -1; //since it may cause out of range error if the input isn't active
			}
			else
			{
				auto* inputBuffer = bufferToFill.buffer->getReadPointer(soloChannel, bufferToFill.startSample);
				auto* outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					outputBuffer[sample] = inputBuffer[sample];  // output one input channel to listen device (WIP)
				}
			}
		}
		else
		{
			auto actualInputChannel = channel % maxInChnnls; // Repeat input channels if more out than in
			if (!activeInChnnls[channel])
			{
				bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
			}
			else
			{
				auto* inputBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel, bufferToFill.startSample);
				auto* outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					outputBuffer[sample] = inputBuffer[sample] * float(masterGain); // More Adjustments will be done later
				}
			}
		}
		float outRMSlvl = bufferToFill.buffer->getRMSLevel(channel, bufferToFill.startSample, bufferToFill.numSamples);
		outputLevelsLinear.at(channel) = outRMSlvl;
	}
}

void MainComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.

	// For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

	// You can add your drawing code here!
	g.setColour(juce::Colour(127, 127, 127));
	int lineThickness = 1;
	int outlineOffset = 3;
	g.drawRect(mstrLevelMeterL.getPosition().getX() - outlineOffset, mstrLevelMeterL.getPosition().getY() - outlineOffset, getWidth() - (OUTER_PAD * 2) + outlineOffset, mstrLevelMeterL.getHeight() + (outlineOffset * 2), lineThickness);
	int mstrLvlMtrHeight = (MASTER_HEIGHT_PRCNT * getHeight()) - (OUTER_PAD);
	int channelPosY = mstrLvlMtrHeight + INNER_PAD + OUTER_PAD;
	int channelOffset = CHANNEL_WIDTH + INNER_PAD;
	int channelHeight = (getHeight() * CHANNEL_HEIGHT_PRCNT) - (OUTER_PAD + INNER_PAD);
	for (int channel = 1; channel <= createdInputs; channel++)
	{
		g.drawVerticalLine(OUTER_PAD + (channelOffset * channel) - (INNER_PAD * 0.5f), channelPosY, channelPosY + channelHeight);
	}
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	int mstrFdrPosX = OUTER_PAD + (INNER_PAD * 3) + MASTER_LVL_MTR_WIDTH;
	int mstrFdrPosY = LABEL_HEIGHT + OUTER_PAD + INNER_PAD;
	int mstFdrLength = (getWidth() * MASTER_WIDTH_PRCNT) - (INNER_PAD + OUTER_PAD + mstrFdrPosX);
	int mstrLvlMtrHeight = (MASTER_HEIGHT_PRCNT * getHeight()) - (OUTER_PAD);

	mstrFdrLabel.setBounds(mstrFdrPosX, OUTER_PAD, 80, LABEL_HEIGHT);
	masterFader.setBounds(mstrFdrPosX, mstrFdrPosY, mstFdrLength, FADER_WIDTH);
	mstrLevelMeterL.setBounds(OUTER_PAD, OUTER_PAD, MASTER_LVL_MTR_WIDTH, mstrLvlMtrHeight);
	mstrLevelMeterR.setBounds(OUTER_PAD + MASTER_LVL_MTR_WIDTH + INNER_PAD, OUTER_PAD, MASTER_LVL_MTR_WIDTH, mstrLvlMtrHeight);

	if (createdInputs <= 0)
	{
		return;
	}
	int channelPosY = mstrLvlMtrHeight + INNER_PAD + OUTER_PAD;
	int channelOffset = CHANNEL_WIDTH + INNER_PAD;
	int channelHeight = (getHeight() * CHANNEL_HEIGHT_PRCNT) - (OUTER_PAD + INNER_PAD);
	for (int channel = 0; channel < createdInputs; channel++)
	{
		int curChOffset = channelOffset * channel;
		inLevelMeters[channel]->setBounds(OUTER_PAD + curChOffset, channelPosY, CHANNEL_LVL_MTR_WIDTH, channelHeight);
		inputFaders[channel]->setBounds(OUTER_PAD + INNER_PAD + CHANNEL_LVL_MTR_WIDTH + curChOffset, channelPosY, FADER_WIDTH, channelHeight);

		int rightChSctnPosX = FADER_WIDTH + OUTER_PAD + (INNER_PAD * 2) + CHANNEL_LVL_MTR_WIDTH;
		EQToggleButtons[channel]->setBounds(rightChSctnPosX + curChOffset, channelPosY, CHANNEL_BTN_WIDTH, CHANNEL_BTN_HEIGHT);
		MuteButtons[channel]->setBounds(rightChSctnPosX + curChOffset, channelPosY + CHANNEL_BTN_HEIGHT, CHANNEL_BTN_WIDTH, CHANNEL_BTN_HEIGHT);
		ListenButtons[channel]->setBounds(rightChSctnPosX + curChOffset, channelPosY + (CHANNEL_BTN_HEIGHT * 2), CHANNEL_BTN_WIDTH, CHANNEL_BTN_HEIGHT);

		int dialStartPosY = channelPosY + (CHANNEL_BTN_HEIGHT * 3) + INNER_PAD;
		HFBoosts[channel]->setBounds(rightChSctnPosX + curChOffset, dialStartPosY, BIG_DIAL_WIDTH, BIG_DIAL_WIDTH + LABEL_HEIGHT);
		LFBoosts[channel]->setBounds(rightChSctnPosX + SMALL_DIAL_WIDTH + curChOffset, dialStartPosY + (BIG_DIAL_WIDTH * 0.5f), BIG_DIAL_WIDTH, BIG_DIAL_WIDTH + LABEL_HEIGHT);
		HFLabels[channel]->setBounds(rightChSctnPosX + curChOffset, dialStartPosY + BIG_DIAL_WIDTH + LABEL_HEIGHT, BIG_DIAL_WIDTH, LABEL_HEIGHT);
		LFLabels[channel]->setBounds(rightChSctnPosX + curChOffset + SMALL_DIAL_WIDTH, dialStartPosY, BIG_DIAL_WIDTH, LABEL_HEIGHT);

		int midFreqDialPosX = rightChSctnPosX + SMALL_DIAL_WIDTH;
		int midFreqStartPosY = dialStartPosY + (BIG_DIAL_WIDTH * 1.5f) + INNER_PAD*1.5f;
		HMFCenterFreq[channel]->setBounds(rightChSctnPosX + curChOffset, midFreqStartPosY, SMALL_DIAL_WIDTH, SMALL_DIAL_WIDTH + LABEL_HEIGHT);
		HMFQLevels[channel]->setBounds(rightChSctnPosX + curChOffset, midFreqStartPosY + SMALL_DIAL_WIDTH + LABEL_HEIGHT, SMALL_DIAL_WIDTH, SMALL_DIAL_WIDTH + LABEL_HEIGHT);
		HMFBoosts[channel]->setBounds(midFreqDialPosX + curChOffset, midFreqStartPosY + (0.5f * (SMALL_DIAL_WIDTH + LABEL_HEIGHT)), BIG_DIAL_WIDTH, BIG_DIAL_WIDTH + LABEL_HEIGHT);
		HMFLabels[channel]->setBounds(rightChSctnPosX + curChOffset + SMALL_DIAL_WIDTH, midFreqStartPosY, BIG_DIAL_WIDTH, LABEL_HEIGHT);

		midFreqStartPosY += INNER_PAD * 1.5f;
		LMFCenterFreq[channel]->setBounds(rightChSctnPosX + curChOffset, midFreqStartPosY + ((SMALL_DIAL_WIDTH + LABEL_HEIGHT) * 2), SMALL_DIAL_WIDTH, SMALL_DIAL_WIDTH + LABEL_HEIGHT);
		LMFQLevels[channel]->setBounds(rightChSctnPosX + curChOffset, midFreqStartPosY + ((SMALL_DIAL_WIDTH + LABEL_HEIGHT) * 3), SMALL_DIAL_WIDTH, SMALL_DIAL_WIDTH + LABEL_HEIGHT);
		LMFBoosts[channel]->setBounds(midFreqDialPosX + curChOffset, midFreqStartPosY + ((SMALL_DIAL_WIDTH + LABEL_HEIGHT) * 2.5f), BIG_DIAL_WIDTH, BIG_DIAL_WIDTH + LABEL_HEIGHT);
		LMFLabels[channel]->setBounds(rightChSctnPosX + curChOffset + SMALL_DIAL_WIDTH, midFreqStartPosY + ((SMALL_DIAL_WIDTH + LABEL_HEIGHT) * 2), BIG_DIAL_WIDTH, LABEL_HEIGHT);
	}
}

//=============================================================================
void MainComponent::sliderValueChanged(juce::Slider* slider) 
{
	if (slider == &masterFader)
	{
		masterGain = juce::Decibels::decibelsToGain(slider->getValue());
		return;
	}
	for (int i = 0; i < createdInputs; i++)
	{
		if (slider == inputFaders[i])
		{
			inputSources[i]->SetGain(juce::Decibels::decibelsToGain(slider->getValue()));
			return;
		}
		else if (slider == HMFBoosts[i] || slider == HMFQLevels[i] || slider == HMFCenterFreq[i])
		{
			if (!inputSources[i]->IsEQEnabled()) { return; }
			double gain = juce::Decibels::decibelsToGain(HMFBoosts[i]->getValue());
			double centerFreq = HMFCenterFreq[i]->getValue() / 1000.; // Since slider is in kHz
			double Q = HMFQLevels[i]->getValue();
			inputSources[i]->ChangeFilter(2, gain, Q, centerFreq);
			return;
		}
		else if (slider == LMFBoosts[i] || slider == LMFQLevels[i] || slider == LMFCenterFreq[i])
		{
			if (!inputSources[i]->IsEQEnabled()) { return; }
			double gain = juce::Decibels::decibelsToGain(LMFBoosts[i]->getValue());
			double centerFreq = LMFCenterFreq[i]->getValue();
			double Q = LMFQLevels[i]->getValue();
			inputSources[i]->ChangeFilter(3, gain, Q, centerFreq);
			return;
		}
		else if (slider == HFBoosts[i])
		{
			if (!inputSources[i]->IsEQEnabled()) { return; }
			double gain = juce::Decibels::decibelsToGain(HFBoosts[i]->getValue());
			//double centerFreq = HFCenterFreq[i]->getValue();
			//double Q = HFQLevels[i]->getValue();
			inputSources[i]->ChangeFilter(0, gain, defaultQ, defaultCutoffHF);
			return;
		}
		else if (slider == LFBoosts[i])
		{
			if (!inputSources[i]->IsEQEnabled()) { return; }
			double gain = juce::Decibels::decibelsToGain(LFBoosts[i]->getValue());
			//double centerFreq = LFCenterFreq[i]->getValue();
			//double Q = LFQLevels[i]->getValue();
			inputSources[i]->ChangeFilter(1, gain, defaultQ, defaultCutoffLF);
			return;
		}
	}

}

void MainComponent::buttonClicked(juce::Button* button)
{
	for (int i = 0; i < createdInputs; i++)
	{
		if (button == EQToggleButtons[i]) { toggleFilter(i); button->setToggleState(inputSources[i]->IsEQEnabled(), juce::dontSendNotification); return; }
		if (button == MuteButtons[i]) { inputSources[i]->SetMuted(!inputSources[i]->IsMuted()); button->setToggleState(inputSources[i]->IsMuted(), juce::dontSendNotification);  return; }
		if (button == ListenButtons[i]) {
			if (soloChannel == i) { 
				button->setToggleState(false, juce::dontSendNotification); soloChannel = -1;
			}
			else {
				button->setToggleState(true, juce::dontSendNotification);
				if (soloChannel >= 0) ListenButtons[soloChannel]->setToggleState(false, juce::dontSendNotification);
				soloChannel = i;
			}
			return;
		}
	}
}

void MainComponent::updateAllFilters()
{
	
	/*double gain;
	double centerFreq;
	double Q;
	for (int i = 0; i < createdChannels; i++)
	{
		gain = juce::Decibels::decibelsToGain(HMFBoosts[i]->getValue());
		centerFreq = HMFCenterFreq[i]->getValue();
		Q = HMFQLevels[i]->getValue();
		HMF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreq, Q, gain));
		HMF_IIR_Filters[i]->reset();

		gain = juce::Decibels::decibelsToGain(LMFBoosts[i]->getValue());
		centerFreq = LMFCenterFreq[i]->getValue();
		Q = LMFQLevels[i]->getValue();
		LMF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreq, Q, gain));
		LMF_IIR_Filters[i]->reset();

		gain = juce::Decibels::decibelsToGain(HFBoosts[i]->getValue());
		HF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makeHighShelf(currentSampleRate, defaultCutoffHF, defaultQ, gain));
		HF_IIR_Filters[i]->reset();

		gain = juce::Decibels::decibelsToGain(LFBoosts[i]->getValue());
		LF_IIR_Filters[i]->setCoefficients(juce::IIRCoefficients::makeLowShelf(currentSampleRate, defaultCutoffLF, defaultQ, gain));
		LF_IIR_Filters[i]->reset();

		if (!enabledChannelEQs[i])
		{
			HMF_IIR_Filters[i]->makeInactive();
			LMF_IIR_Filters[i]->makeInactive();
			HF_IIR_Filters[i]->makeInactive();
			LF_IIR_Filters[i]->makeInactive();
		}
	}*/
}

void MainComponent::toggleFilter(int channel)
{
	inputSources[channel]->SetEnabledEQ(!inputSources[channel]->IsEQEnabled());
	/*if (!inputSources[channel]->IsEQEnabled())
	{
		HF_IIR_Filters[channel]->makeInactive();
		LF_IIR_Filters[channel]->makeInactive();
		HMF_IIR_Filters[channel]->makeInactive();
		LMF_IIR_Filters[channel]->makeInactive();
	}
	else
	{
		double gain;
		double centerFreq;
		double Q;
		gain = juce::Decibels::decibelsToGain(HMFBoosts[channel]->getValue());
		centerFreq = HMFCenterFreq[channel]->getValue();
		Q = HMFQLevels[channel]->getValue();
		HMF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreq, Q, gain));
		HMF_IIR_Filters[channel]->reset();

		gain = juce::Decibels::decibelsToGain(LMFBoosts[channel]->getValue());
		centerFreq = LMFCenterFreq[channel]->getValue();
		Q = LMFQLevels[channel]->getValue();
		LMF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate, centerFreq, Q, gain));
		LMF_IIR_Filters[channel]->reset();

		gain = juce::Decibels::decibelsToGain(HFBoosts[channel]->getValue());
		HF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makeHighShelf(currentSampleRate, defaultCutoffHF, defaultQ, gain));
		HF_IIR_Filters[channel]->reset();

		gain = juce::Decibels::decibelsToGain(LFBoosts[channel]->getValue());
		LF_IIR_Filters[channel]->setCoefficients(juce::IIRCoefficients::makeLowShelf(currentSampleRate, defaultCutoffLF, defaultQ, gain));
		LF_IIR_Filters[channel]->reset();
	}*/
}
