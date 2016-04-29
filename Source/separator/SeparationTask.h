/*
  ==============================================================================

    SeparationThread.h
    Created: 10 Mar 2016 1:22:51pm
    Author:  Sean

	This class will execute the separation process for a whole file (non-realtime).
	It will run on it's own thread, and the progress will be displayed in a progress window while the 
	operation is ongoing.

  ==============================================================================
*/

#ifndef SEPARATIONTHREAD_H_INCLUDED
#define SEPARATIONTHREAD_H_INCLUDED
#include "JuceHeader.h"
#include "HarmonicPercussiveSeparator.h"

class SeparationTask : public ThreadWithProgressWindow
{
public:
	/*
		Constructor
		Inputs: 
				- pointer to AudioFormatReader
				- file name as a String
				- sampleRate as a double
	*/
	SeparationTask(AudioFormatReader* readerForFile, String fileName, double sampleRate);
	~SeparationTask();

	/*
		Over ridden method from ThreadWithProgressWindow.
		Starts the Thread.
	*/
	void run() override;
private:
	ScopedPointer<MedianSeparator> separator;
};



#endif  // SEPARATIONTHREAD_H_INCLUDED
