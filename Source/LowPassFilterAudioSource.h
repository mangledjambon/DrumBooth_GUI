/*
  ==============================================================================

    LowPassFilterAudioSource.h
    Created: 23 Feb 2016 1:33:33pm
    Author:  Sean

  ==============================================================================
*/

#ifndef LOWPASSFILTERAUDIOSOURCE_H_INCLUDED
#define LOWPASSFILTERAUDIOSOURCE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class LowPassFilterAudioSource : public AudioSource
{
public:
	LowPassFilterAudioSource(AudioSource* source, double sampleRate);
	~LowPassFilterAudioSource();

	// inherited from AudioSource
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	// filter modifier methods
	void setLowPassFilterFrequency(double newFrequency);
	double getLowPassFilterFrequency();
	void setEnabled(bool enabled);

private:
	bool enabled;
	double frequency;
	double currentSampleRate;
	ScopedPointer<IIRFilterAudioSource> lowPassFilter;
};



#endif  // LOWPASSFILTERAUDIOSOURCE_H_INCLUDED
