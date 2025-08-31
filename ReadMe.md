## Tools Used 
- C++, Language
- JUCE, Audio Processing Application Framework (Might be all that is needed?)
- Projucer, Initial Project setup and export.

## Multiple Audio Source Architecture
- Have a MixerAudioSource
- Make new class inheriting AudioSource, with its own overridden functions 
- Have an array of inherited AudioSource type to store all audio sources
- decouple input channels from GUI initialization, change it to have one GUI input channel component per AudioSource
- Add input sources (AudioSource) whenever an input device/source is added, connect it to AudioSource object
- Refactor getNextAudioBlock to pull from all the AudioSources to apply respective gains and EQs, then have the MixerAudioSource put them all together.
- Refactor filters to be stereo (need two filters per input)
- Double check for any other necessary refactors