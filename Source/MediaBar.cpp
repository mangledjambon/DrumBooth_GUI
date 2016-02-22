/*
  ==============================================================================

    MediaBar.cpp
    Created: 14 Feb 2016 11:16:06pm
    Author:  Sean

  ==============================================================================
*/

#include "MediaBar.h"
#include "TransportState.h"
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
MediaBar::MediaBar(AudioTransportSource& transport) : transportSource(transport)
{
	addAndMakeVisible(label_Gain = new Label("Gain"));
	label_Gain->setText("Volume:", dontSendNotification);

	// Add Buttons, might move to Main Component
	addAndMakeVisible(button_LoadFile = new TextButton("Load"));
	addAndMakeVisible(button_PlayPause = new TextButton("PlayPause"));
	addAndMakeVisible(button_Stop = new TextButton("Stop"));

	// set up Gain slider
	addAndMakeVisible(slider_Gain = new Slider("Gain"));
	slider_Gain->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	slider_Gain->setRange(0, 100, 1);
	slider_Gain->setValue(100);
	slider_Gain->addListener(this);

	// begin timer
	startTimer(500);
}

MediaBar::~MediaBar()
{}

void MediaBar::paint (Graphics& g)
{
	Rectangle<int> localBounds = getLocalBounds();
	Rectangle<int> sliderArea = localBounds.removeFromLeft(getWidth() / 15);
	Rectangle<int> labelArea = sliderArea.removeFromTop(20);

	// draw slider and label
	slider_Gain->setBounds(sliderArea);
	label_Gain->setBounds(labelArea);


	g.fillAll(Colours::slategrey);   // clear the background

	g.setColour(Colours::whitesmoke);
	g.drawRect(localBounds, 1);   // draw an outline around the component
	
	// draw buttons
	const int BUTTON_WIDTH = localBounds.getWidth() / 3;	// set button width as constant int value (1/3 of localBound's width)
	Rectangle<int> buttonArea = localBounds.reduced(10).removeFromBottom(getHeight() / 10);
	button_LoadFile->setBounds(buttonArea.removeFromLeft(BUTTON_WIDTH));
	button_PlayPause->setBounds(buttonArea.removeFromLeft(BUTTON_WIDTH));
	button_Stop->setBounds(buttonArea.removeFromLeft(BUTTON_WIDTH));

	g.setColour(Colours::whitesmoke);

	/* 
		This code displays the time elapsed in the track followed by the total 
		length of the track, in minutes and seconds. It also displays the current 
		status of the playback (Playing, Paused, etc...)		
	*/

	int min_elapsed = (transportSource.getNextReadPosition() / 44100) / 60;
	int sec_elapsed = (transportSource.getNextReadPosition() / 44100) % 60;
	int min_total = (transportSource.getTotalLength() / 44100) / 60;
	int sec_total = (transportSource.getTotalLength() / 44100) % 60;

	g.drawFittedText(
		  String(min_elapsed)+ "m "
		+ String(sec_elapsed) + "s \t\t-\t\t" 
		+ String(min_total) + "m " + String(sec_total) + "s"
		+ "\nStatus: " + getPlaybackStatus(), 
		localBounds, Justification::centred, 3);

}

void MediaBar::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void MediaBar::timerCallback()
{
	repaint();
}

void MediaBar::setPlaybackState(int state)
{
	playbackState = state;
}

String MediaBar::getPlaybackStatus()
{
	switch (playbackState)
	{
	case Stopped:
		button_PlayPause->setButtonText("Play");
		button_Stop->setEnabled(false);
		return "Stopped";
		break;

	case Starting:
		return "Starting";
		break;

	case Playing:
		button_PlayPause->setButtonText("Pause");
		button_Stop->setEnabled(true);
		return "Playing";
		break;

	case Pausing:
		return "Pausing";
		break;

	case Paused:
		button_PlayPause->setButtonText("Resume");
		return "Paused";
		break;

	case Stopping:
		return "Stopping";
		break;
	}
}

void MediaBar::sliderValueChanged(Slider* sliderThatWasChanged)
{
	if (sliderThatWasChanged == slider_Gain)
	{
		float gain = 0.0f;
		gain = slider_Gain->getValue() / 100;

		transportSource.setGain(gain);
	}
}

void MediaBar::addButtonListeners(Button::Listener* listenerToAdd)
{
	button_LoadFile->addListener(listenerToAdd);
	button_PlayPause->addListener(listenerToAdd);
	button_Stop->addListener(listenerToAdd);
}