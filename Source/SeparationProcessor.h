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

class Separator : public AudioProcessor
{

public:
	Separator();
	~Separator();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

private:
	ScopedPointer<IIRFilter> highPassFilter, lowPassFilter;
};



#endif  // SEPARATIONPROCESSOR_H_INCLUDED
