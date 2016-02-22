/*
  ==============================================================================

    SeparationProcessor.cpp
    Created: 21 Feb 2016 10:35:53pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationProcessor.h"

Separator::Separator()
{}

Separator::~Separator()
{}

void Separator::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(sampleRate, 2000));
}

void Separator::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
	{
		float* buffer = bufferToFill.buffer->getWritePointer(channel);

		highPassFilter->processSamples(buffer, bufferToFill.numSamples);
	}
}

void Separator::releaseResources()
{}