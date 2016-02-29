/*
  ==============================================================================

    InfoBar.cpp
    Created: 11 Feb 2016 11:49:34am
    Author:  Sean

  ==============================================================================
*/

#include "InfoBar.h"

InfoBar::InfoBar()
{

	addAndMakeVisible(label_cpuUsage = new Label("CPU"));
	addAndMakeVisible(label_sampleRate = new Label("SampleRate"));
	addAndMakeVisible(label_bufferSize = new Label("BlockSize"));

	startTimer(500);
}

InfoBar::~InfoBar()
{
}

void InfoBar::paint(Graphics& g)
{
	g.fillAll(Colours::lightgrey);
	Rectangle<int> localBounds = getLocalBounds();
	Rectangle<int> left = localBounds.removeFromLeft(getWidth() / 3);
	Rectangle<int> centre = localBounds.removeFromLeft(getWidth() / 3);
	Rectangle<int> right = localBounds;

	label_cpuUsage->setBounds(left);
	label_cpuUsage->setJustificationType(Justification::centred);
	label_sampleRate->setBounds(centre);
	label_sampleRate->setJustificationType(Justification::centred);
	label_bufferSize->setBounds(right);
	label_bufferSize->setJustificationType(Justification::centred);
}

void InfoBar::resized()
{
}

void InfoBar::updateCpu(double newValue)
{
	cpuUsage = newValue;
}

void InfoBar::updateSampleRate(double newValue)
{
	sampleRate = newValue;
}

void InfoBar::updateBufferSize(int newValue)
{
	bufferSize = newValue;
}

void InfoBar::timerCallback()
{
	label_cpuUsage->setText("CPU %: " + String(cpuUsage), dontSendNotification);
	label_sampleRate->setText("Sample Rate: " + String(sampleRate), dontSendNotification);
	label_bufferSize->setText("Buffer size: " + String(bufferSize), dontSendNotification);
}