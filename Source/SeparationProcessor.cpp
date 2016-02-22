/*
  ==============================================================================

    SeparationProcessor.cpp
    Created: 21 Feb 2016 10:35:53pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationProcessor.h"

Separator::Separator()
{
	highPassFilter = new IIRFilter();
}

Separator::~Separator()
{}

void Separator::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(sampleRate,10000));
}

void Separator::releaseResources()
{}