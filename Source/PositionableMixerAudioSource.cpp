/*
  ==============================================================================

    PositionableMixerAudioSource.cpp
    Created: 22 Mar 2016 2:10:50pm
    Author:  Sean

  ==============================================================================
*/

#include "PositionableMixerAudioSource.h"

PositionableMixerAudioSource::PositionableMixerAudioSource() 
	:	tempBuffer(2, 0),
		track1Gain(0.5f),
		track2Gain(0.5f),
		currentSampleRate(0.0),
		bufferSizeExpected(0),
		currentPlayingPosition(0),
		isPlayingLoop(false)
{}

PositionableMixerAudioSource::~PositionableMixerAudioSource()
{
	//removeAllInputs();
}

void PositionableMixerAudioSource::addInputSource(GainAudioFormatReaderSource* newInput, const bool deleteWhenRemoved)
{
	if (newInput != nullptr && !inputs.contains(newInput))
	{
		lock.enter();
		double localSampleRate = currentSampleRate;
		int localBufferSize = bufferSizeExpected;
		lock.exit();

		if (localSampleRate > 0.0)
			newInput->prepareToPlay(localBufferSize, localSampleRate);

		const ScopedLock sl(lock);

		inputsToDelete.setBit(inputs.size(), deleteWhenRemoved);
		inputs.add(newInput);
	}
}

void PositionableMixerAudioSource::removeInputSource(GainAudioFormatReaderSource* inputToRemove, const bool deleteSource)
{
	if (inputToRemove != nullptr)
	{
		lock.enter();
		const int index = inputs.indexOf(inputToRemove);

		if (index >= 0)
		{
			inputsToDelete.shiftBits(index, 1);
			inputs.remove(index);
		}

		lock.exit();

		if (index >= 0)
		{
			inputToRemove->releaseResources();

			if (deleteSource)
				delete inputToRemove;
		}
	}
}

void PositionableMixerAudioSource::removeAllInputs()
{
	lock.enter();
	Array<GainAudioFormatReaderSource*> inputsCopy(inputs);
	BigInteger inputsToDeleteCopy(inputsToDelete);
	inputs.clear();
	lock.exit();

	for (int i = inputsCopy.size(); --i >= 0;)
	{
		if (inputsToDeleteCopy[i] != 0)
			delete inputsCopy[i];
	}
}

// PositionableAudioSource methods
void PositionableMixerAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	tempBuffer.setSize(2, samplesPerBlockExpected);

	const ScopedLock sl(lock);

	currentSampleRate = sampleRate;
	bufferSizeExpected = samplesPerBlockExpected;

	for (int i = inputs.size(); --i >= 0;)
	{
		inputs.getUnchecked(i)->prepareToPlay(samplesPerBlockExpected, sampleRate);
	}
}

void PositionableMixerAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	const ScopedLock sl(lock);

	if (inputs.size() > 0)
	{
		// causes artifacts in audio when gain applied here
		//bufferToFill.buffer->applyGain(track1Gain);
		inputs.getUnchecked(0)->getNextAudioBlock(bufferToFill);

		if (inputs.size() > 1)
		{
			tempBuffer.setSize(jmax(1, bufferToFill.buffer->getNumChannels()), bufferToFill.buffer->getNumSamples());
			AudioSourceChannelInfo buffer2(&tempBuffer, 0, bufferToFill.numSamples);

			for (int i = 1; i < inputs.size(); ++i)
			{
				inputs.getUnchecked(i)->getNextAudioBlock(buffer2);
				//buffer2.buffer->applyGain(track1Gain);

				for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
				{
					bufferToFill.buffer->addFrom(channel, bufferToFill.startSample, tempBuffer, channel, 0, bufferToFill.numSamples, track2Gain);
				}
			}
		}
	}
	else // no inputs, so clear buffer
	{
		bufferToFill.clearActiveBufferRegion();
	}
}

void PositionableMixerAudioSource::releaseResources()
{
	const ScopedLock sl(lock);

	for (int i = inputs.size(); --i >= 0;)
		inputs.getUnchecked(i)->releaseResources();

	tempBuffer.setSize(2, 0);

	currentSampleRate = 0;
	bufferSizeExpected = 0;
}

void PositionableMixerAudioSource::setNextReadPosition(int64 newReadPosition)
{
	currentPlayingPosition = newReadPosition;

	for (int i = inputs.size(); --i >= 0;)
	{
		inputs.getUnchecked(i)->setNextReadPosition(newReadPosition);
	}
}

int64 PositionableMixerAudioSource::getNextReadPosition() const
{
	return currentPlayingPosition;
}

int64 PositionableMixerAudioSource::getTotalLength() const
{
	const ScopedLock sl(lock);

	int64 maxLength = 0;

	for (int i = inputs.size(); --i >= 0;)
		maxLength = jmax(maxLength, inputs.getUnchecked(i)->getTotalLength());

	return maxLength;
}

bool PositionableMixerAudioSource::isLooping() const
{
	return isPlayingLoop;
}

GainAudioFormatReaderSource* PositionableMixerAudioSource::getInput(int index)
{
	if (index < 2)
	{
		return inputs.getUnchecked(index);
	}
	
	return nullptr;
}

void PositionableMixerAudioSource::applyGain(float track1, float track2)
{
	track1Gain = track1;
	track2Gain = track2;
}