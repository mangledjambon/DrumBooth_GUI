/*
  ==============================================================================

    SeparationThread.cpp
    Created: 10 Mar 2016 1:22:51pm
    Author:  Sean

  ==============================================================================
*/

#include "SeparationTask.h"

SeparationTask::SeparationTask() : ThreadWithProgressWindow("Separator running...", true, true, 5000, "Cancel")
{
	separator = new MedianSeparator();
}

SeparationTask::~SeparationTask()
{}


