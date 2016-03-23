/*
  ==============================================================================

    HarmonicPercussiveSeparator.h
    Created: 22 Mar 2016 1:00:45pm
    Author:  Sean

	This class handles performing the median filtered separation on the audio file.
	The filtering technique is taken from Derry Fitzgerald's paper entitled
	'Harmonic Percussive Separation using Median Filtering' from DAFx-10.

	https://www.researchgate.net/publication/254583990_HarmonicPercussive_Separation_using_Median_Filtering
  ==============================================================================
*/

#ifndef HARMONICPERCUSSIVESEPARATOR_H_INCLUDED
#define HARMONICPERCUSSIVESEPARATOR_H_INCLUDED

#include "JuceHeader.h"
#include "../separator/ArraySorter.h"
#include "../transforms/STFT.h"
#include "../transforms/ISTFT.h"
#include "../Eigen/Eigen"
#include "../ffft/FFTReal.h"

class MedianSeparator
{
public:
	MedianSeparator(AudioFormatReader* reader);
	~MedianSeparator();

	void fillBuffer();
	void convertToSpectrogram();
	void filterBins();
	void filterFrames();
	void resynthesize();
	void writeFiles();

	String fileNameNoExt;
	double sampleRate;

private:
	int numSamples, numCols, numChannels, startSample;
	AudioSampleBuffer samples;
	AudioFormatReader* reader;
	MatrixXcf spectrogram[2];
	MatrixXf filteredSpectrogram_P[2];
	MatrixXf filteredSpectrogram_H[2];
	MatrixXcf resynthSpectrogram_P[2];
	MatrixXcf resynthSpectrogram_H[2];

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MedianSeparator)
};



#endif  // HARMONICPERCUSSIVESEPARATOR_H_INCLUDED
