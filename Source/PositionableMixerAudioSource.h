/*
  ==============================================================================

    PositionableMixerAudioSource.h
    Created: 22 Mar 2016 2:10:50pm
    Author:  Sean

  ==============================================================================
*/

#ifndef POSITIONABLEMIXERAUDIOSOURCE_H_INCLUDED
#define POSITIONABLEMIXERAUDIOSOURCE_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class PositionableMixerAudioSource : public PositionableAudioSource
{
public:
	PositionableMixerAudioSource();
	~PositionableMixerAudioSource();

	// MixerAudioSource methods
	void addInputSource(AudioTransportSource* newInput, const bool deleteWhenRemoved);
	void removeInputSource(AudioTransportSource* inputToRemove, const bool deleteSource);
	void removeAllInputs();

	void setPosition(double newPosition);

	// PositionableAudioSource methods
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void setNextReadPosition(int64 newReadPosition) override;
	int64 getNextReadPosition() const override;
	int64 getTotalLength() const override;
	bool isLooping() const override;

	void applyGain(float track1, float track2);
	void start();
	void stop();

	AudioTransportSource* getInput(int index);

private:
	float track1Gain, track2Gain;
	Array<AudioTransportSource*> inputs;
	BigInteger inputsToDelete;
	CriticalSection lock;
	AudioSampleBuffer tempBuffer;

	int64 currentPlayingPosition;
	double currentSampleRate;
	int bufferSizeExpected;
	bool isPlayingLoop;
};


#endif  // POSITIONABLEMIXERAUDIOSOURCE_H_INCLUDED
