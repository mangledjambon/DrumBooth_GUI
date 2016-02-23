/*
  ==============================================================================

    SeparationProcessor.cpp
    Created: 21 Feb 2016 10:35:53pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationSource.h"

Separator::Separator(AudioSource* source)
{
	highPassFilter = new IIRFilterAudioSource(source, false);
}

Separator::~Separator()
{}

void Separator::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;
	highPassFilter->prepareToPlay(samplesPerBlockExpected, sampleRate);
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(sampleRate,filterFrequency));
	setFilterFrequency(20);
}

void Separator::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	highPassFilter->getNextAudioBlock(bufferToFill);
}

void Separator::releaseResources()
{
	highPassFilter->releaseResources();
}

void Separator::setFilterFrequency(int newFrequency)
{
	filterFrequency = newFrequency;
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(currentSampleRate, filterFrequency));
}

int Separator::getFilterFrequency()
{
	return filterFrequency;
}