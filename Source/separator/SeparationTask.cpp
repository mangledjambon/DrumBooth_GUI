/*
  ==============================================================================

    SeparationThread.cpp
    Created: 10 Mar 2016 1:22:51pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationTask.h"

SeparationTask::SeparationTask(AudioFormatReader* readerForFile, String fileName) : 
	ThreadWithProgressWindow("Separator running...", true, true, 10000, "Cancel")
{
	separator = new MedianSeparator(readerForFile);
	separator->fileNameNoExt = fileName;
}

SeparationTask::~SeparationTask()
{
	//signalThreadShouldExit();
}

void SeparationTask::run()
{
	if (threadShouldExit())
		return;

	setStatusMessage("Reading audio data into buffer.");
	separator->fillBuffer();
	setProgress(0.1);

	if (threadShouldExit())
		return;

	setStatusMessage("Converting to spectrogram.");
	separator->convertToSpectrogram();
	setProgress(0.3);

	if (threadShouldExit())
		return;

	setStatusMessage("Filtering frequency bins to get isolated harmonics.");
	separator->filterBins();
	setProgress(0.5);

	if (threadShouldExit())
		return;

	setStatusMessage("Filtering time frames to get isolated percussion.");
	separator->filterFrames();
	setProgress(0.65);

	if (threadShouldExit())
		return;

	setStatusMessage("Resynthesizing back to audio.");
	separator->resynthesize();
	setProgress(0.8);

	if (threadShouldExit())
		return;

	setStatusMessage("Writing to .wav files");
	separator->writeFiles();
	setProgress(1.0);
}


