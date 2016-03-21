/*
  ==============================================================================

    SeparationThread.h
    Created: 10 Mar 2016 1:22:51pm
    Author:  Sean

	This class will execute the separation process for a whole file (non-realtime).
	It will run on it's own thread, and the progress will be displayed in a progress window while the 
	operation is ongoing.

	TODO: Consult PanRev for appropriate constructor to use.

  ==============================================================================
*/

#ifndef SEPARATIONTHREAD_H_INCLUDED
#define SEPARATIONTHREAD_H_INCLUDED
#include "JuceHeader.h"

class SeparationTask
{
public:
	SeparationTask();
	~SeparationTask();

};



#endif  // SEPARATIONTHREAD_H_INCLUDED
