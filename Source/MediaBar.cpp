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
MediaBar::MediaBar(AudioTransportSource& transport, PositionableMixerAudioSource& mixer) : transportSource(transport), mixerSource(mixer)
{

	// Add Buttons
	addAndMakeVisible(button_LoadFile = new TextButton("Load"));
	addAndMakeVisible(button_PlayPause = new TextButton("PlayPause"));
	addAndMakeVisible(button_Stop = new TextButton("Stop"));
	addAndMakeVisible(button_Process = new TextButton("Process Audio"));
	addAndMakeVisible(button_spectrogramEnabled = new ToggleButton("Spectrogram On/Off"));
	
	// set process button disabled
	button_Process->setEnabled(false);

	// set toggle button to checked (true)
	button_spectrogramEnabled->setToggleState(true, dontSendNotification);

	// set up Gain slider label
	addAndMakeVisible(label_Gain = new Label("Gain"));
	label_Gain->setText("Volume:", dontSendNotification);

	// set up filter labels
	addAndMakeVisible(label_SeparationControl = new Label("Separation Control"));
	label_SeparationControl->setText("Separation:", dontSendNotification);

	// set up volume slider
	addAndMakeVisible(slider_Gain = new Slider("Gain"));
	slider_Gain->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	slider_Gain->setRange(0, 100, 1);
	slider_Gain->setValue(100);
	slider_Gain->addListener(this);


	// set up filter sliders
	addAndMakeVisible(slider_SeparationControl = new Slider("Separation control knob"));
	slider_SeparationControl->setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	slider_SeparationControl->setTextBoxStyle(Slider::NoTextBox, true, 50, 20);
	slider_SeparationControl->setRange(0, 1.0f, 0.01f);
	slider_SeparationControl->setValue(0.5f);
	//slider_SeparationControl->setEnabled(false);

	// begin timer
	startTimerHz(5);
}

MediaBar::~MediaBar()
{}

void MediaBar::paint (Graphics& g)
{
	g.fillAll(Colours::slategrey);   // clear the background

	Rectangle<int> localBounds = getLocalBounds().reduced(5); // get area

	g.setColour(Colours::darkblue);
	g.drawRect(localBounds, 1);   // draw an outline around the component

	const int BUTTON_WIDTH = localBounds.getWidth() / 8;	// set button width as constant int value (1/8 of localBound's width)
	const int BUTTON_HEIGHT = localBounds.getHeight() / 10;

	// draw volume sliders and labels
	Rectangle<int> volumeSliderArea = localBounds.removeFromLeft(getWidth() / 15);
	Rectangle<int> volumeLabelArea = volumeSliderArea.removeFromTop(25);
	slider_Gain->setBounds(volumeSliderArea.reduced(5));
	label_Gain->setBounds(volumeLabelArea);

	// draw separation control label and slider
	Rectangle<int> filterSliderArea = localBounds.removeFromLeft(getWidth()/10);
	Rectangle<int> filterLabelArea = filterSliderArea.removeFromTop(25);
	label_SeparationControl->setBounds(filterLabelArea.reduced(5));
	slider_SeparationControl->setBounds(filterSliderArea);
	
	// draw buttons

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
	int64 min_elapsed, sec_elapsed, min_total, sec_total;

	if (mixerSource.getTotalLength() != 0)
	{
		min_elapsed = (mixerSource.getNextReadPosition() / 44100) / 60;
		sec_elapsed = (mixerSource.getNextReadPosition() / 44100) % 60;
		min_total = (mixerSource.getTotalLength() / 44100) / 60;
		sec_total = (mixerSource.getTotalLength() / 44100) % 60;
	}
	else
	{
		min_elapsed = (transportSource.getNextReadPosition() / 44100) / 60;
		sec_elapsed = (transportSource.getNextReadPosition() / 44100) % 60;
		min_total = (transportSource.getTotalLength() / 44100) / 60;
		sec_total = (transportSource.getTotalLength() / 44100) % 60;
	}

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

		// only enable when transportSource has data
		if (transportSource.getTotalLength() > 0)
			button_Process->setEnabled(true);

		return "Stopped";
		break;

	case Starting:
		return "Starting";
		break;

	case Playing:
		button_PlayPause->setButtonText("Pause");
		button_Stop->setEnabled(true);
		button_spectrogramEnabled->setEnabled(true);
		button_Process->setEnabled(false);
		return "Playing";
		break;

	case Pausing:
		return "Pausing";
		break;

	case Paused:
		button_PlayPause->setButtonText("Resume");
		button_Process->setEnabled(true);
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
	// Volume slider changed
	if (sliderThatWasChanged == slider_Gain)
	{
		float gain = 0.0f; // initialise variable to hold gain
		gain = slider_Gain->getValue() / 100;	// get gain value from slider

		transportSource.setGain(gain);
	}
	// Separation slider changed
	else if (sliderThatWasChanged == slider_SeparationControl)
	{
		float mix = slider_SeparationControl->getValue(); // get value from separation slider

		mixerSource.applyGain(mix, (1.0f - mix));	// apply gain to tracks in mixer (track1Gain + track2Gain = 1.0f)
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
	slider_SeparationControl->addListener(listenerToAdd);
}