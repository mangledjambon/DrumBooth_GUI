/*
  ==============================================================================

    SeparationProcessor.h
    Created: 21 Feb 2016 10:35:53pm
    Author:  Sean

  ==============================================================================
*/

#ifndef SEPARATIONPROCESSOR_H_INCLUDED
#define SEPARATIONPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class Separator : public AudioSource
{

public:
	Separator(AudioSource* source);
	~Separator();

	// inherited from AudioSource
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void setFilterFrequency(int newFrequency);
	int getFilterFrequency();
private:
	int currentSampleRate;
	int filterFrequency;
	ScopedPointer<IIRFilterAudioSource> highPassFilter;
};



#endif  // SEPARATIONPROCESSOR_H_INCLUDED
