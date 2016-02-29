/*
  ==============================================================================

    LowPassFilterAudioSource.cpp
    Created: 23 Feb 2016 1:33:33pm
    Author:  Sean

  ==============================================================================
*/

#include "LowPassFilterAudioSource.h"

LowPassFilterAudioSource::LowPassFilterAudioSource(AudioSource* source, double sampleRate)
	:	currentSampleRate(sampleRate)
{
	lowPassFilter = new IIRFilterAudioSource(source, false);
}

LowPassFilterAudioSource::~LowPassFilterAudioSource()
{}

void LowPassFilterAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;

	lowPassFilter->prepareToPlay(samplesPerBlockExpected, sampleRate);
	setLowPassFilterFrequency(15000);
	lowPassFilter->setCoefficients(IIRCoefficients::makeLowPass(sampleRate, frequency));
}

void LowPassFilterAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	if (enabled)
		lowPassFilter->getNextAudioBlock(bufferToFill);
}

void LowPassFilterAudioSource::releaseResources()
{
	lowPassFilter->releaseResources();
}

double LowPassFilterAudioSource::getLowPassFilterFrequency()
{
	return frequency;
}

void LowPassFilterAudioSource::setLowPassFilterFrequency(double newFrequency)
{
	frequency = newFrequency;
	lowPassFilter->setCoefficients(IIRCoefficients::makeLowPass(currentSampleRate, frequency));
}

void LowPassFilterAudioSource::setEnabled(bool newValue)
{
	enabled = newValue;
}
