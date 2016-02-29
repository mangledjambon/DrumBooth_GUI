/*
  ==============================================================================

    SeparationProcessor.cpp
    Created: 21 Feb 2016 10:35:53pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationSource.h"

Separator::Separator(AudioSource* source)
{}

Separator::~Separator()
{}

void Separator::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	currentSampleRate = sampleRate;	
}

void Separator::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{}

void Separator::releaseResources()
{}