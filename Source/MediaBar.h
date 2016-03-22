/*
  ==============================================================================

    MediaBar.h
    Created: 14 Feb 2016 11:16:06pm
    Author:  Sean

  ==============================================================================
*/

#ifndef MEDIABAR_H_INCLUDED
#define MEDIABAR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MediaBar    : public Component,
					public Timer,
					public Slider::Listener
{
public:
    MediaBar(AudioTransportSource& transport);
    ~MediaBar();

    void paint (Graphics&);
    void resized();

	void setPlaybackState(int state);
	void timerCallback() override;

	void sliderValueChanged(Slider* sliderThatWasChanged) override;

	void addButtonListeners(Button::Listener* listenerToAdd);
	void addSliderListeners(Slider::Listener* listenerToAdd);

	void setTrackInfo(String name);

	ScopedPointer<TextButton> button_LoadFile, button_PlayPause, button_Stop, button_Process;
	ScopedPointer<Slider> slider_HighPassFilterFreq;
	ScopedPointer<ToggleButton> button_spectrogramEnabled;

private:
	String getPlaybackStatus();

	// Audio
	int playbackState;
	AudioTransportSource& transportSource;

	// GUI
	String artistName, trackName, fileName;
	ScopedPointer<Label> label_artistName, label_trackName;
	ScopedPointer<Label> label_Gain;
	ScopedPointer<Slider> slider_Gain;
	ScopedPointer<Label> label_HighPassFilterFreq;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MediaBar)
};


#endif  // MEDIABAR_H_INCLUDED
