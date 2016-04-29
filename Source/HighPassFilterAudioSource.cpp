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
	// create new IIRFilterAudioSource for AudioSource source
	highPassFilter = new IIRFilterAudioSource(source, false);
}

HighPassFilterAudioSource::~HighPassFilterAudioSource()
{}

void HighPassFilterAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;
	highPassFilter->prepareToPlay(samplesPerBlockExpected, sampleRate);

	// set filter frequency to 20Hz
	setHighPassFilterFrequency(20);

	// apply high pass filter coefficients to frequency
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(sampleRate, frequency));
}

void HighPassFilterAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	// if filter is enabled, pass audio buffer to high pass filter
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
	// get new frequency
	frequency = newFrequency;

	// set new coefficients for frequency
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
