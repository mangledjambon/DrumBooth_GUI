/*
  ==============================================================================

    GainAudioFormatReaderSource.cpp
    Created: 23 Mar 2016 11:55:23am
    Author:  Sean

  ==============================================================================
*/

#include "GainAudioFormatReaderSource.h"

/*
==============================================================================

==============================================================================
*/

GainAudioFormatReaderSource::GainAudioFormatReaderSource(AudioFormatReader* const r,
	const bool deleteReaderWhenThisIsDeleted)
	: reader(r, deleteReaderWhenThisIsDeleted),
	nextPlayPos(0),
	looping(false)
{
	jassert(reader != nullptr);
}

GainAudioFormatReaderSource::~GainAudioFormatReaderSource() {}

int64 GainAudioFormatReaderSource::getTotalLength() const { return reader->lengthInSamples; }
void GainAudioFormatReaderSource::setNextReadPosition(int64 newPosition) { nextPlayPos = newPosition; }
void GainAudioFormatReaderSource::setLooping(bool shouldLoop) { looping = shouldLoop; }

int64 GainAudioFormatReaderSource::getNextReadPosition() const
{
	return looping ? nextPlayPos % reader->lengthInSamples
		: nextPlayPos;
}

void GainAudioFormatReaderSource::prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/) {}
void GainAudioFormatReaderSource::releaseResources() {}

void GainAudioFormatReaderSource::getNextAudioBlock(const AudioSourceChannelInfo& info)
{

	if (info.numSamples > 0)
	{
		const int64 start = nextPlayPos;

		if (looping)
		{
			const int64 newStart = start % reader->lengthInSamples;
			const int64 newEnd = (start + info.numSamples) % reader->lengthInSamples;

			if (newEnd > newStart)
			{
				reader->read(info.buffer, info.startSample,
					(int)(newEnd - newStart), newStart, true, true);
			}
			else
			{
				const int endSamps = (int)(reader->lengthInSamples - newStart);

				reader->read(info.buffer, info.startSample,
					endSamps, newStart, true, true);

				reader->read(info.buffer, info.startSample + endSamps,
					(int)newEnd, 0, true, true);
			}

			nextPlayPos = newEnd;
		}
		else
		{
			reader->read(info.buffer, info.startSample,
				info.numSamples, start, true, true);
			nextPlayPos += info.numSamples;
		}
	}
}
