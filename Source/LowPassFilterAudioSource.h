/*
  ==============================================================================

    LowPassFilterAudioSource.h
    Created: 23 Feb 2016 1:33:33pm
    Author:  Sean

	This class applies a low pass filter to an AudioSource.

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

	// inherited from AudioSource ============================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;
	// =======================================================

	// filter modifier methods ===============================

	// set new low pass frequency for filter
	void setLowPassFilterFrequency(double newFrequency);

	// get current filter frequency
	double getLowPassFilterFrequency();

	// enable/disable filter
	void setEnabled(bool enabled);

	// =======================================================

private:
	bool enabled;
	double frequency;
	double currentSampleRate;
	ScopedPointer<IIRFilterAudioSource> lowPassFilter;
};



#endif  // LOWPASSFILTERAUDIOSOURCE_H_INCLUDED
