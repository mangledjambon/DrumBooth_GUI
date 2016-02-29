/*
  ==============================================================================

    HighPassFilterAudioSource.cpp
    Created: 23 Feb 2016 1:33:19pm
    Author:  Sean

  ==============================================================================
*/

#include "HighPassFilterAudioSource.h"

HighPassFilterAudioSource::HighPassFilterAudioSource(AudioSource* source)
{
	highPassFilter = new IIRFilterAudioSource(source, false);
}

HighPassFilterAudioSource::~HighPassFilterAudioSource()
{}

void HighPassFilterAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;
	highPassFilter->prepareToPlay(samplesPerBlockExpected, sampleRate);
	setHighPassFilterFrequency(20);
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(sampleRate, frequency));
}

void HighPassFilterAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	if (enabled)
		highPassFilter->getNextAudioBlock(bufferToFill);
}

void HighPassFilterAudioSource::releaseResources()
{
	highPassFilter->releaseResources();
}

// filter modifier methods
void HighPassFilterAudioSource::setHighPassFilterFrequency(double newFrequency)
{
	frequency = newFrequency;
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(currentSampleRate, frequency));
}

double HighPassFilterAudioSource::getHighPassFilterFrequency()
{
	return frequency;
}

void HighPassFilterAudioSource::setEnabled(bool newValue)
{
	enabled = newValue;
}
