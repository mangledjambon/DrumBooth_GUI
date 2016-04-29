/*
  ==============================================================================

    HighPassFilterAudioSource.h
    Created: 23 Feb 2016 1:33:19pm
    Author:  Sean

	This class applies a high pass filter to an AudioSource.

  ==============================================================================
*/

#ifndef HIGHPASSFILTERAUDIOSOURCE_H_INCLUDED
#define HIGHPASSFILTERAUDIOSOURCE_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class HighPassFilterAudioSource : public AudioSource
{
public:
	HighPassFilterAudioSource(AudioSource* source);
	~HighPassFilterAudioSource();

	// inherited from AudioSource ============================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;
	// =======================================================

	// filter modifier methods ===============================

	// Set high pass filter frequency (20 - 20000)
	void setHighPassFilterFrequency(double newFrequency);

	// return current filter frequency
	double getHighPassFilterFrequency();

	// enable/disable filter
	void setEnabled(bool enabled);

	//=======================================================
private:
	bool enabled;
	double currentSampleRate;
	double frequency;
	ScopedPointer<IIRFilterAudioSource> highPassFilter;
};



#endif  // HIGHPASSFILTERAUDIOSOURCE_H_INCLUDED
