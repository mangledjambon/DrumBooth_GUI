/*
  ==============================================================================

    GainAudioFormatReaderSource.h
    Created: 23 Mar 2016 11:55:23am
    Author:  Sean

  ==============================================================================
*/

#ifndef GAINAUDIOFORMATREADERSOURCE_H_INCLUDED
#define GAINAUDIOFORMATREADERSOURCE_H_INCLUDED

/*
==============================================================================

==============================================================================
*/

#ifndef JUCE_GAINAUDIOFORMATREADERSOURCE_H_INCLUDED
#define JUCE_GAINAUDIOFORMATREADERSOURCE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
/**
A type of AudioSource that will read from an AudioFormatReader and has changeable Gain settings.

@see PositionableAudioSource, AudioTransportSource, BufferingAudioSource
*/
class GainAudioFormatReaderSource : public PositionableAudioSource
{
public:
	//==============================================================================
	/** Creates an AudioFormatReaderSource for a given reader.

	@param sourceReader                     the reader to use as the data source - this must
	not be null
	@param deleteReaderWhenThisIsDeleted    if true, the reader passed-in will be deleted
	when this object is deleted; if false it will be
	left up to the caller to manage its lifetime
	*/
	GainAudioFormatReaderSource(AudioFormatReader* sourceReader,
		bool deleteReaderWhenThisIsDeleted);

	/** Destructor. */
	~GainAudioFormatReaderSource();

	//==============================================================================
	/** Toggles loop-mode.

	If set to true, it will continuously loop the input source. If false,
	it will just emit silence after the source has finished.

	@see isLooping
	*/
	void setLooping(bool shouldLoop) override;

	/** Returns whether loop-mode is turned on or not. */
	bool isLooping() const override { return looping; }

	/** Returns the reader that's being used. */
	AudioFormatReader* getAudioFormatReader() const noexcept { return reader; }

	//==============================================================================
	/** Implementation of the AudioSource method. */
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

	/** Implementation of the AudioSource method. */
	void releaseResources() override;

	/** Implementation of the AudioSource method. */
	void getNextAudioBlock(const AudioSourceChannelInfo&) override;

	//==============================================================================
	/** Implements the PositionableAudioSource method. */
	void setNextReadPosition(int64 newPosition) override;

	/** Implements the PositionableAudioSource method. */
	int64 getNextReadPosition() const override;

	/** Implements the PositionableAudioSource method. */
	int64 getTotalLength() const override;

private:
	//==============================================================================
	OptionalScopedPointer<AudioFormatReader> reader;

	int64 volatile nextPlayPos;
	bool volatile looping;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioFormatReaderSource)
};


#endif   // JUCE_AUDIOFORMATREADERSOURCE_H_INCLUDED




#endif  // GAINAUDIOFORMATREADERSOURCE_H_INCLUDED
