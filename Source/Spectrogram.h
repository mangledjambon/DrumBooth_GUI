/*
  ==============================================================================

    Spectrogram.h
    Created: 19 Feb 2016 3:51:44pm
    Author:  Sean

  ==============================================================================
*/

#ifndef SPECTROGRAM_H_INCLUDED
#define SPECTROGRAM_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class SpectrogramComponent :	public AudioAppComponent,
								private Timer
{
public:
	SpectrogramComponent()
		: forwardFFT(fftOrder, false),
		spectrogramImage(Image::RGB, 512, 512, true),
		fifoIndex(0),
		nextFFTBlockReady(false),
		enabled(false)
	{
		setOpaque(true);
		setAudioChannels(2, 0);  // we want a couple of input channels but no outputs
		startTimerHz(60);
		//setSize(700, 500);
	}

	~SpectrogramComponent()
	{
		shutdownAudio();
	}

	//=======================================================================
	void prepareToPlay(int /*samplesPerBlockExpected*/, double /*newSampleRate*/) override
	{
		// (nothing to do here)
	}
	
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		if (bufferToFill.buffer->getNumChannels() > 0)
		{
			const float* channelData = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);

			for (int i = 0; i < bufferToFill.numSamples; ++i)
				pushNextSampleIntoFifo(channelData[i]);
		}
	}

	void releaseResources() override
	{
		// (nothing to do here)
	}

	//=======================================================================
	void paint(Graphics& g) override
	{
		if (!enabled)
		{
			g.fillAll(Colours::whitesmoke);
			g.drawFittedText("(Spectrogram disabled)", getLocalBounds(), Justification::centred, 1);
			return;
		}

		g.fillAll(Colours::lightslategrey);

		g.setOpacity(1.0f);
		g.drawImageWithin(spectrogramImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit);
	}

	void timerCallback() override
	{

		if (nextFFTBlockReady && enabled)
		{
			drawNextLineOfSpectrogram();
			nextFFTBlockReady = false;
			repaint();
		}
	}

	void pushNextSampleIntoFifo(float sample) noexcept
	{
		// if the fifo contains enough data, set a flag to say
		// that the next line should now be rendered..
		if (fifoIndex == fftSize)
		{
			if (!nextFFTBlockReady)
			{
				zeromem(fftData, sizeof(fftData));
				memcpy(fftData, fifo, sizeof(fifo));
				nextFFTBlockReady = true;
			}

			fifoIndex = 0;
		}

		fifo[fifoIndex++] = sample;
	}

	void drawNextLineOfSpectrogram()
	{
		const int rightHandEdge = spectrogramImage.getWidth() - 1;
		const int imageHeight = spectrogramImage.getHeight();

		// first, shuffle our image leftwards by 1 pixel..
		spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

		// then render our FFT data..
		forwardFFT.performFrequencyOnlyForwardTransform(fftData);

		// find the range of values produced, so we can scale our rendering to
		// show up the detail clearly
		Range<float> maxLevel = FloatVectorOperations::findMinAndMax(fftData, fftSize*2);

		for (int y = 1; y < imageHeight; ++y)
		{
			const float skewedProportionY = 1.0f - std::exp(std::log(y / (float)imageHeight) * 0.2f);
			const int fftDataIndex = jlimit(0, fftSize*2, (int)(skewedProportionY * (fftSize/3)));
			const float level = jmap(fftData[fftDataIndex], 0.0f, maxLevel.getEnd(), 0.0f, 1.0f);

			spectrogramImage.setPixelAt(rightHandEdge, y, Colour::fromHSV(level, 1.0f, level, 1.0f));
		}
	}

	void setEnabled(bool status)
	{
		enabled = status;

		if (!enabled)
			repaint();
	}

	void clear(Graphics& g)
	{
		g.fillAll(Colours::black);
	}

	void setToClear(bool newValue)
	{
		toBeCleared = newValue;
	}

	enum
	{
		fftOrder = 10,
		fftSize = 2 << fftOrder
	};

private:
	FFT forwardFFT;
	Image spectrogramImage;

	bool enabled;
	bool toBeCleared;
	float fifo[fftSize];
	float fftData[2 * fftSize];
	int fifoIndex;
	bool nextFFTBlockReady;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};



#endif  // SPECTROGRAM_H_INCLUDED
