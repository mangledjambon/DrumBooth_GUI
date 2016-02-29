/*
  ==============================================================================

    InfoBar.h
    Created: 11 Feb 2016 11:49:34am
    Author:  Sean

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"

#ifndef INFOBAR_H_INCLUDED
#define INFOBAR_H_INCLUDED

class InfoBar : public Component,
				public Timer
{

public:

	InfoBar();
	~InfoBar();

	void updateCpu(double newValue);
	void updateSampleRate(double newValue);
	void updateBufferSize(int newValue);

	void paint(Graphics& g) override;
	void resized() override;

	void timerCallback() override;

private:
	double sampleRate, cpuUsage;
	int bufferSize;
	ScopedPointer<Label> label_cpuUsage, label_sampleRate, label_bufferSize;
};



#endif  // INFOBAR_H_INCLUDED
