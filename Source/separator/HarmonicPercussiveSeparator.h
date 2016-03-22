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

#include "../transforms/STFT.h"
#include "../transforms/ISTFT.h"
#include "../Eigen/Eigen"
#include "../ffft/FFTReal.h"

class MedianSeparator
{
public:
	MedianSeparator();
	~MedianSeparator();

private:
};



#endif  // HARMONICPERCUSSIVESEPARATOR_H_INCLUDED
