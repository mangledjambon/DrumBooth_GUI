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

void Separator::prepareToPlay(double sampleRate, int samplesPerBlockExpected)
{
	highPassFilter->setCoefficients(IIRCoefficients::makeHighPass(sampleRate,5000));
}

void Separator::processBlock(AudioBuffer<double> &buffer, MidiBuffer &midiMessages)
{}

void Separator::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
	for (int channel = 0; channel < buffer.getNumChannels(); channel++)
	{
		float* channelData = buffer.getWritePointer(channel);
		highPassFilter->processSamples(channelData, buffer.getNumSamples());
	}
}

void Separator::releaseResources()
{}

const String Separator::getName() const
{
	return "Harmonic/Percussive Separator";
}

void Separator::processBlockBypassed(AudioBuffer<double> &buffer, MidiBuffer &midiMessages) {}
void Separator::processBlockBypassed(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) {}

bool Separator::setPreferredBusArrangement(bool isInputBus, int busIndex, const AudioChannelSet &preferredSet) 
{
	return false;
}

bool Separator::supportsDoublePrecisionProcessing() const 
{
	return false;
}

bool Separator::silenceInProducesSilenceOut() const 
{
	return true;
}

double Separator::getTailLengthSeconds() const 
{
	return 0.0;
}

bool Separator::acceptsMidi() const 
{
	return false;
}

bool Separator::producesMidi() const 
{
	return false;
}

AudioProcessorEditor* Separator::createEditor() 
{
	return nullptr;
}

bool Separator::hasEditor() const 
{
	return false;
}

int Separator::getNumPrograms() 
{
	return 0;
}

int Separator::getCurrentProgram() 
{
	return 0;
}

void Separator::setCurrentProgram(int index) {}

const String Separator::getProgramName(int index) 
{
	return "";
}

void Separator::changeProgramName(int index, const String &newName) {}
void Separator::getStateInformation(juce::MemoryBlock &destData) {}
void Separator::setStateInformation(const void *data, int sizeInBytes)
{}