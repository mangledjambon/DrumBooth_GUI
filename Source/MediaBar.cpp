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

	// Add Buttons
	addAndMakeVisible(button_LoadFile = new TextButton("Load"));
	addAndMakeVisible(button_PlayPause = new TextButton("PlayPause"));
	addAndMakeVisible(button_Stop = new TextButton("Stop"));
	addAndMakeVisible(button_Process = new TextButton("Process Audio"));
	addAndMakeVisible(button_spectrogramEnabled = new ToggleButton("Spectrogram On/Off"));
	button_Process->setEnabled(false);

	// set toggle button to checked (true)
	button_spectrogramEnabled->setToggleState(true, dontSendNotification);

	// set up Gain slider label
	addAndMakeVisible(label_Gain = new Label("Gain"));
	label_Gain->setText("Volume:", dontSendNotification);

	// set up filter labels
	// high pass
	addAndMakeVisible(label_HighPassFilterFreq = new Label("High Pass Filter Frequency"));
	label_HighPassFilterFreq->setText("High Pass Freq:", dontSendNotification);

	// set up volume slider
	addAndMakeVisible(slider_Gain = new Slider("Gain"));
	slider_Gain->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	slider_Gain->setRange(0, 100, 1);
	slider_Gain->setValue(100);
	slider_Gain->addListener(this);


	// set up filter sliders
	addAndMakeVisible(slider_HighPassFilterFreq = new Slider("Filter Frequency"));
	slider_HighPassFilterFreq->setSliderStyle(Slider::SliderStyle::LinearBar);
	slider_HighPassFilterFreq->setRange(20, 20000, 2);
	slider_HighPassFilterFreq->setValue(20);

	// begin timer
	startTimerHz(5);
}

MediaBar::~MediaBar()
{}

void MediaBar::paint (Graphics& g)
{
	g.fillAll(Colours::slategrey);   // clear the background

	Rectangle<int> localBounds = getLocalBounds().reduced(5); // get area

	// draw filter sliders and labels
	Rectangle<int> filterSliderArea = localBounds.removeFromBottom(30).reduced(2);
	Rectangle<int> filterLabelArea = filterSliderArea.removeFromLeft(80);
	label_HighPassFilterFreq->setBounds(filterLabelArea);
	slider_HighPassFilterFreq->setBounds(filterSliderArea);

	g.setColour(Colours::darkblue);
	g.drawRect(localBounds, 1);   // draw an outline around the component
	
	// draw buttons
	const int BUTTON_WIDTH = localBounds.getWidth() / 8;	// set button width as constant int value (1/3 of localBound's width)
	
	Rectangle<int> sliderArea = localBounds.removeFromLeft(getWidth() / 15);
	Rectangle<int> labelArea = sliderArea.removeFromTop(20);
	slider_Gain->setBounds(sliderArea.reduced(5));
	label_Gain->setBounds(labelArea);

	Rectangle<int> buttonArea = localBounds.reduced(10).removeFromBottom(getHeight() / 10);
	button_LoadFile->setBounds(buttonArea.removeFromLeft(BUTTON_WIDTH));
	button_PlayPause->setBounds(buttonArea.removeFromLeft(BUTTON_WIDTH));
	button_Stop->setBounds(buttonArea.removeFromLeft(BUTTON_WIDTH));
	button_spectrogramEnabled->setBounds(buttonArea.removeFromRight(BUTTON_WIDTH));
	button_Process->setBounds(buttonArea.removeFromRight(BUTTON_WIDTH * 2));


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
		fileName + "\n"
		+ String(min_elapsed)+ "m "
		+ String(sec_elapsed) + "s \t\t-\t\t" 
		+ String(min_total) + "m " + String(sec_total) + "s"
		+ "\nStatus: " + getPlaybackStatus(), 
		localBounds, Justification::centred, 4);

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
		button_spectrogramEnabled->setEnabled(false);
		return "Stopped";
		break;

	case Starting:
		return "Starting";
		break;

	case Playing:
		button_PlayPause->setButtonText("Pause");
		button_Stop->setEnabled(true);
		button_spectrogramEnabled->setEnabled(true);
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

void MediaBar::setTrackInfo(String name)
{
	fileName = name;
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
	button_spectrogramEnabled->addListener(listenerToAdd);
	button_Process->addListener(listenerToAdd);
}

void MediaBar::addSliderListeners(Slider::Listener* listenerToAdd)
{
	slider_HighPassFilterFreq->addListener(listenerToAdd);
	//slider_LowPassFilterFreq->addListener(listenerToAdd);
}