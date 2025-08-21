#include "MainComponent.h"
#include "LevelMeter.h"

//==============================================================================
MainComponent::MainComponent()
{
	// Make sure you set the size of the component after
	// you add any child components.
	int winWidth = 800;
	int winHeight = 600;
	int inChannelAmt = 3;

	outputLevelsLinear.resize(2);// stereo only (for now)
	masterGain = juce::Decibels::decibelsToGain(defaultdBFS);
	inputLevelsLinear.resize(inChannelAmt);
	channelGains.resize(inChannelAmt);
	mutedChannels.resize(inChannelAmt);

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

	for (int channel = 0; channel < inChannelAmt; channel++, createdChannels++)
	{
		juce::Slider* fader = new juce::Slider();
		channelFaders.add(fader);
		channelFaders[channel]->setSliderStyle(juce::Slider::LinearVertical);
		channelFaders[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		channelFaders[channel]->setRange(-100.0, 0.0, 0.05);
		channelFaders[channel]->setSkewFactorFromMidPoint(defaultMidPointdBFS);
		channelFaders[channel]->setPopupDisplayEnabled(true, false, this);
		channelFaders[channel]->setTextValueSuffix(" dB");
		channelFaders[channel]->setValue(defaultdBFS);
		channelFaders[channel]->addListener(this);
		addAndMakeVisible(channelFaders[channel]);

		juce::IIRFilter filterHF;
		juce::IIRCoefficients coef1;
		coef1.makeHighShelf(currentSampleRate, defaultCutoffHF, defaultQ, 1.);
		filterHF.setCoefficients(coef1);
		HF_IIR_Filters.add(&filterHF);

		juce::Slider* dial1 = new juce::Slider();
		HFBoosts.add(dial1);
		HFBoosts[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HFBoosts[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		HFBoosts[channel]->setRange(-15.0, 15.0, 0.05);
		HFBoosts[channel]->setPopupDisplayEnabled(true, false, this);
		HFBoosts[channel]->setTextValueSuffix(" dB");
		HFBoosts[channel]->setValue(0.0);
		HFBoosts[channel]->addListener(this);
		addAndMakeVisible(HFBoosts[channel]);

		juce::IIRFilter filterLF;
		juce::IIRCoefficients coef2;
		coef2.makeHighShelf(currentSampleRate, defaultCutoffLF, defaultQ, 1.);
		filterLF.setCoefficients(coef2);
		LF_IIR_Filters.add(&filterLF);

		juce::Slider* dial2 = new juce::Slider();
		LFBoosts.add(dial2);
		LFBoosts[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LFBoosts[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		LFBoosts[channel]->setRange(-15.0, 15.0, 0.05);
		LFBoosts[channel]->setPopupDisplayEnabled(true, false, this);
		LFBoosts[channel]->setTextValueSuffix(" dB");
		LFBoosts[channel]->setValue(0.0);
		LFBoosts[channel]->addListener(this);
		addAndMakeVisible(LFBoosts[channel]);

		juce::TextButton* focusButton = new juce::TextButton();
		FocusButtons.add(focusButton);
		FocusButtons[channel]->setButtonText("Focus");
		FocusButtons[channel]->setInterceptsMouseClicks(true, false);
		FocusButtons[channel]->setState(juce::Button::buttonNormal);
		FocusButtons[channel]->addListener(this);
		addAndMakeVisible(FocusButtons[channel]);

		juce::TextButton* muteButton = new juce::TextButton();
		MuteButtons.add(muteButton);
		mutedChannels.at(channel) = false;
		MuteButtons[channel]->setButtonText("Mute");
		MuteButtons[channel]->setInterceptsMouseClicks(true, false);
		MuteButtons[channel]->setState(juce::Button::buttonNormal);
		MuteButtons[channel]->addListener(this);
		addAndMakeVisible(MuteButtons[channel]);

		juce::TextButton* listenButton = new juce::TextButton();
		ListenButtons.add(listenButton);
		ListenButtons[channel]->setButtonText("Solo");
		ListenButtons[channel]->setInterceptsMouseClicks(true, false);
		ListenButtons[channel]->setState(juce::Button::buttonNormal);
		ListenButtons[channel]->addListener(this);
		addAndMakeVisible(ListenButtons[channel]);

		juce::IIRFilter filterHMF;
		juce::IIRCoefficients coef3;
		coef3.makePeakFilter(currentSampleRate, defaultFreqHMF, defaultQ, defaultBoostGain);
		filterHMF.setCoefficients(coef3);
		HMF_IIR_Filters.add(&filterHMF);

		juce::Slider* dial3 = new juce::Slider();
		HMFBoosts.add(dial3);
		HMFBoosts[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HMFBoosts[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		HMFBoosts[channel]->setRange(-15.0, 15.0, 0.05);
		HMFBoosts[channel]->setPopupDisplayEnabled(true, false, this);
		HMFBoosts[channel]->setTextValueSuffix(" dB");
		HMFBoosts[channel]->setValue(0.0);
		HMFBoosts[channel]->addListener(this);
		addAndMakeVisible(HMFBoosts[channel]);

		juce::Slider* dial4 = new juce::Slider();
		HMFQLevels.add(dial4);
		HMFQLevels[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HMFQLevels[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		HMFQLevels[channel]->setRange(0.01, 10.0, 0.01);
		HMFQLevels[channel]->setPopupDisplayEnabled(true, false, this);
		HMFQLevels[channel]->setTextValueSuffix(" Q");
		HMFQLevels[channel]->setValue(2.0);
		HMFQLevels[channel]->addListener(this);
		addAndMakeVisible(HMFQLevels[channel]);

		juce::Slider* dial5 = new juce::Slider();
		HMFCenterFreq.add(dial5);
		HMFCenterFreq[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		HMFCenterFreq[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		HMFCenterFreq[channel]->setRange(00.450, 20.000, 0.001); // y = 4887.5 * x^2 + 450, x = (0, 2)
		HMFCenterFreq[channel]->setPopupDisplayEnabled(true, false, this);
		HMFCenterFreq[channel]->setTextValueSuffix(" kHz");
		HMFCenterFreq[channel]->setSkewFactorFromMidPoint(defaultFreqHMF / 1000);
		HMFCenterFreq[channel]->setValue(defaultFreqHMF / 1000.);
		HMFCenterFreq[channel]->addListener(this);
		addAndMakeVisible(HMFCenterFreq[channel]);

		juce::IIRFilter filterLMF;
		juce::IIRCoefficients coef4;
		coef4.makePeakFilter(currentSampleRate, defaultFreqLMF, defaultQ, defaultBoostGain);
		filterLMF.setCoefficients(coef4);
		LMF_IIR_Filters.add(&filterLMF);

		juce::Slider* dial6 = new juce::Slider();
		LMFBoosts.add(dial6);
		LMFBoosts[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LMFBoosts[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		LMFBoosts[channel]->setRange(-15.0, 15.0, 0.05);
		LMFBoosts[channel]->setPopupDisplayEnabled(true, false, this);
		LMFBoosts[channel]->setTextValueSuffix(" dB");
		LMFBoosts[channel]->setValue(0.0);
		LMFBoosts[channel]->addListener(this);
		addAndMakeVisible(LMFBoosts[channel]);

		juce::Slider* dial7 = new juce::Slider();
		LMFQLevels.add(dial7);
		LMFQLevels[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LMFQLevels[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		LMFQLevels[channel]->setRange(0.01, 10.0, 0.01);
		LMFQLevels[channel]->setPopupDisplayEnabled(true, false, this);
		LMFQLevels[channel]->setTextValueSuffix(" Q");
		LMFQLevels[channel]->setValue(2.0);
		LMFQLevels[channel]->addListener(this);
		addAndMakeVisible(LMFQLevels[channel]);

		juce::Slider* dial8 = new juce::Slider();
		LMFCenterFreq.add(dial8);
		LMFCenterFreq[channel]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
		LMFCenterFreq[channel]->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		LMFCenterFreq[channel]->setRange(50.0, 1500.0, 0.001); // y = 362.5 * x^2 + 50, x = (0, 2)
		LMFCenterFreq[channel]->setPopupDisplayEnabled(true, false, this);
		LMFCenterFreq[channel]->setTextValueSuffix(" Hz");
		LMFCenterFreq[channel]->setSkewFactorFromMidPoint(450.0);
		LMFCenterFreq[channel]->setValue(450.0);
		LMFCenterFreq[channel]->addListener(this);
		addAndMakeVisible(LMFCenterFreq[channel]);

		LevelMeter* inLevelMeter = new LevelMeter([channel, this]() -> float {return juce::Decibels::gainToDecibels(inputLevelsLinear.at(channel)); }, true);
		inLevelMeters.add(inLevelMeter);
		addAndMakeVisible(inLevelMeters[channel]);
	}

	setSize(winWidth, winHeight);


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

	if (createdChannels > 0)
	{
		updateAllFilters(); 
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
	
	// Input Channels
	for (auto channel = 0; channel < maxInChnnls; ++channel)
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
			}
			float inRMSlvl = bufferToFill.buffer->getRMSLevel(channel, bufferToFill.startSample, bufferToFill.numSamples);
			inputLevelsLinear.at(channel) = inRMSlvl;
		}
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

}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	mstrFdrLabel.setBounds(50, 10, 80, 20);
	masterFader.setBounds(50, 25, getWidth() - 100, 20);
	mstrLevelMeterL.setBounds(10, 10, 15, 40);
	mstrLevelMeterR.setBounds(30, 10, 15, 40);

	if (createdChannels <= 0)
	{
		return;
	}
	int channelOffset = 180;
	for (int channel = 0; channel < createdChannels; channel++)
	{
		inLevelMeters[channel]->setBounds(15 + (channelOffset * channel), 50, 20, getHeight() - 100);

		channelFaders[channel]->setBounds(50 + (channelOffset * channel), 50, 20, getHeight() - 100);

		FocusButtons[channel]->setBounds(75 + (channelOffset * channel), 50, 80, 30);
		MuteButtons[channel]->setBounds(75 + (channelOffset * channel), 80, 80, 30);
		ListenButtons[channel]->setBounds(75 + (channelOffset * channel), 110, 80, 30);

		HFBoosts[channel]->setBounds(75 + (channelOffset * channel), 140, 65, 65);
		LFBoosts[channel]->setBounds(75 + (channelOffset * channel), 200, 65, 65);

		HMFCenterFreq[channel]->setBounds(75 + (channelOffset * channel), 260, 50, 50);
		HMFQLevels[channel]->setBounds(75 + (channelOffset * channel), 300, 50, 50);
		HMFBoosts[channel]->setBounds(115 + (channelOffset * channel), 280, 65, 65);

		LMFCenterFreq[channel]->setBounds(75 + (channelOffset * channel), 350, 50, 50);
		LMFQLevels[channel]->setBounds(75 + (channelOffset * channel), 390, 50, 50);
		LMFBoosts[channel]->setBounds(115 + (channelOffset * channel), 370, 65, 65);
	}
}

void MainComponent::sliderValueChanged(juce::Slider* slider) 
{
	if (slider == &masterFader)
	{
		masterGain = juce::Decibels::decibelsToGain(slider->getValue());
		return;
	}
	for (int i = 0; i < createdChannels; i++)
	{
		if (slider == channelFaders[i])
		{
			channelGains[i] = juce::Decibels::decibelsToGain(slider->getValue());
			return;
		}
		else if (slider == HMFBoosts[i] || slider == HMFQLevels[i] || slider == HMFCenterFreq[i])
		{
			double gain = juce::Decibels::decibelsToGain(HMFBoosts[i]->getValue());
			double centerFreq = HMFCenterFreq[i]->getValue() * 1000.; // Since slider is in kHz
			double Q = HMFQLevels[i]->getValue();
			juce::IIRCoefficients coef;
			coef.makePeakFilter(currentSampleRate, centerFreq, Q, gain);
			HMF_IIR_Filters[i]->setCoefficients(coef);
			return;
		}
		else if (slider == LMFBoosts[i] || slider == LMFQLevels[i] || slider == LMFCenterFreq[i])
		{
			double gain = juce::Decibels::decibelsToGain(LMFBoosts[i]->getValue());
			double centerFreq = LMFCenterFreq[i]->getValue();
			double Q = LMFQLevels[i]->getValue();
			juce::IIRCoefficients coef;
			coef.makePeakFilter(currentSampleRate, centerFreq, Q, gain);
			LMF_IIR_Filters[i]->setCoefficients(coef);
			return;
		}
		else if (slider == HFBoosts[i])
		{
			double gain = juce::Decibels::decibelsToGain(HFBoosts[i]->getValue());
			juce::IIRCoefficients coef;
			coef.makeHighShelf(currentSampleRate, defaultCutoffHF, defaultQ, gain);
			HF_IIR_Filters[i]->setCoefficients(coef);
			return;
		}
		else if (slider == LFBoosts[i])
		{
			double gain = juce::Decibels::decibelsToGain(LFBoosts[i]->getValue());
			juce::IIRCoefficients coef;
			coef.makeHighShelf(currentSampleRate, defaultCutoffLF, defaultQ, gain);
			LF_IIR_Filters[i]->setCoefficients(coef);
			return;
		}
	}

}

void MainComponent::buttonClicked(juce::Button* button)
{
	for (int i = 0; i < createdChannels; i++)
	{
		if (button == FocusButtons[i]) {/*Focus Channel*/; return; }
		if (button == MuteButtons[i]) { mutedChannels[i] = !mutedChannels[i]; return; }
		if (button == ListenButtons[i]) { soloChannel == i ? soloChannel = -1 : soloChannel = i; return; }
	}
}

void MainComponent::updateAllFilters()
{
	double gain;
	double centerFreq;
	double Q;
	for (int i = 0; i < createdChannels; i++)
	{
		gain = juce::Decibels::decibelsToGain(HMFBoosts[i]->getValue());
		centerFreq = HMFCenterFreq[i]->getValue();
		Q = HMFQLevels[i]->getValue();
		juce::IIRCoefficients coef1;
		coef1.makePeakFilter(currentSampleRate, centerFreq, Q, gain);
		HMF_IIR_Filters[i]->setCoefficients(coef1);

		gain = juce::Decibels::decibelsToGain(LMFBoosts[i]->getValue());
		centerFreq = LMFCenterFreq[i]->getValue();
		Q = LMFQLevels[i]->getValue();
		juce::IIRCoefficients coef2;
		coef2.makePeakFilter(currentSampleRate, centerFreq, Q, gain);
		LMF_IIR_Filters[i]->setCoefficients(coef2);

		gain = juce::Decibels::decibelsToGain(HFBoosts[i]->getValue());
		juce::IIRCoefficients coef3;
		coef3.makeHighShelf(currentSampleRate, defaultCutoffHF, defaultQ, gain);
		HF_IIR_Filters[i]->setCoefficients(coef3);

		gain = juce::Decibels::decibelsToGain(LFBoosts[i]->getValue());
		juce::IIRCoefficients coef4;
		coef4.makeHighShelf(currentSampleRate, defaultCutoffLF, defaultQ, gain);
		LF_IIR_Filters[i]->setCoefficients(coef4);
	}
}
