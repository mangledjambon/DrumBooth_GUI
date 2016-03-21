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
	stft = new STFT(2048);
	stft->initWindow(1);

	istft = new ISTFT();
	istft->initWindow(1);
}

Separator::~Separator()
{}

void Separator::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;	
}

void Separator::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	// perform separation here
	for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
	{
		std::complex<float>* complexData = new std::complex<float>[2049];

		const float* bufferData = bufferToFill.buffer->getWritePointer(channel);
		float* fftData = stft->performForwardTransform(bufferData);
		complexData = stft->realToComplex(fftData, 2049);
	}
}

void Separator::releaseResources()
{}