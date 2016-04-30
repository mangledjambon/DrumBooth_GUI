/*
  ==============================================================================

    MediaBar.h
    Created: 14 Feb 2016 11:16:06pm
    Author:  Sean

	This class creates the component which holds the play/pause/stop buttons and the volume/separation control sliders.

  ==============================================================================
*/

#ifndef MEDIABAR_H_INCLUDED
#define MEDIABAR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PositionableMixerAudioSource.h"

class MediaBar    : public Component,
					public Timer,
					public Slider::Listener
{
public:
    MediaBar(AudioTransportSource& transport, PositionableMixerAudioSource& mixer);
    ~MediaBar();

    void paint (Graphics&);
    void resized();

	/*
		Set the State that will be displayed in the audio player. 
	*/
	void setPlaybackState(int state);

	/*
		Set track name to be displayed in player.
	*/
	void setTrackInfo(String name);

	// Timer method
	void timerCallback() override;

	// Slider::Listener method
	void sliderValueChanged(Slider* sliderThatWasChanged) override;

	// add listeners
	void addButtonListeners(Button::Listener* listenerToAdd);
	void addSliderListeners(Slider::Listener* listenerToAdd);

	// TextButtons for media controls
	ScopedPointer<TextButton> button_LoadFile, button_PlayPause, button_Stop, button_Process;

	// rotary slider for separation
	ScopedPointer<Slider> slider_SeparationControl;

	// toggle button for spectogram enabled/disabled
	ScopedPointer<ToggleButton> button_spectrogramEnabled;

private:
	String getPlaybackStatus();

	// Audio
	int playbackState;
	AudioTransportSource& transportSource;
	PositionableMixerAudioSource& mixerSource;

	// GUI
	String artistName, trackName, fileName;
	ScopedPointer<Label> label_artistName, label_trackName;
	ScopedPointer<Label> label_Gain;
	ScopedPointer<Slider> slider_Gain;
	ScopedPointer<Label> label_SeparationControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MediaBar)
};


#endif  // MEDIABAR_H_INCLUDED
