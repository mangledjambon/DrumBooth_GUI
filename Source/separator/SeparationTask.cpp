/*
  ==============================================================================

    SeparationThread.cpp
    Created: 10 Mar 2016 1:22:51pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationTask.h"

SeparationTask::SeparationTask(AudioFormatReader* readerForFile, String fileName, double rate) : 
	ThreadWithProgressWindow("Separator running...", true, true, 10000, "Cancel")
{
	// create new MedianSeparator for file from AudioFormatReader*
	separator = new MedianSeparator(readerForFile);
	separator->fileNameNoExt = fileName;
	separator->sampleRate = rate;
}

SeparationTask::~SeparationTask()
{
	//signalThreadShouldExit();
}

void SeparationTask::run()
{
	if (threadShouldExit())
		return;

	setStatusMessage("Reading audio data into buffer. (1/6)");
	separator->fillBuffer();
	setProgress(-1);

	if (threadShouldExit())
		return;

	setStatusMessage("Converting to spectrogram. (2/6)");
	separator->convertToSpectrogram();
	//setProgress(-1);

	if (threadShouldExit())
		return;

	setStatusMessage("Filtering frequency bins to get isolated harmonics. (3/6)");
	separator->filterBins();
	//setProgress(0.5);

	if (threadShouldExit())
		return;

	setStatusMessage("Filtering time frames to get isolated percussion. (4/6)");
	separator->filterFrames();
	//setProgress(0.65);

	if (threadShouldExit())
		return;

	setStatusMessage("Resynthesizing back to audio. (5/6)");
	separator->resynthesize();
	//setProgress(0.8);

	if (threadShouldExit())
		return;

	setStatusMessage("Writing to .wav files (6/6)");
	separator->writeFiles();
	//setProgress(1.0);
}


