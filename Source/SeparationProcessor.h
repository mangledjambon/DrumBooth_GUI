/*
  ==============================================================================

    SeparationProcessor.h
    Created: 21 Feb 2016 10:35:53pm
    Author:  Sean

  ==============================================================================
*/

#ifndef SEPARATIONPROCESSOR_H_INCLUDED
#define SEPARATIONPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class Separator : public AudioProcessor
{

public:
	Separator();
	~Separator();

	// inherited from AudioProcessor
	void prepareToPlay(double sampleRate, int samplesPerBlockExpected) override;
	void releaseResources() override;
	const String getName() const override;
	void processBlock(AudioBuffer<double> &buffer, MidiBuffer &midiMessages) override;
	void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
	void processBlockBypassed(AudioBuffer<double> &buffer, MidiBuffer &midiMessages) override;
	void processBlockBypassed(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;
	bool setPreferredBusArrangement(bool isInputBus, int busIndex, const AudioChannelSet &preferredSet) override;
	bool supportsDoublePrecisionProcessing() const override;
	bool silenceInProducesSilenceOut() const override;
	double getTailLengthSeconds() const override;
	bool acceptsMidi() const override;
	bool producesMidi() const override;
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String &newName) override;
	void getStateInformation(juce::MemoryBlock &destData) override;
	void setStateInformation(const void *data, int sizeInBytes) override;

private:
	ScopedPointer<IIRFilter> highPassFilter, lowPassFilter;
};



#endif  // SEPARATIONPROCESSOR_H_INCLUDED
