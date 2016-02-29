/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "DrumBooth_Components.h"
using juce::Rectangle;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   :	public AudioAppComponent,
								public ApplicationCommandTarget,
								public MenuBarModel,
								public ChangeListener,
								public Button::Listener,
								public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent() : state(Stopped)
    {
        setSize (800, 600);
		setWantsKeyboardFocus(true);

		// Audio setup
        // specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
		formatManager.registerBasicFormats();

		// listen for changes to transportSource's state
		transportSource.addChangeListener(this);

		//GUI - add components here
		addAndMakeVisible(infoBar = new InfoBar());						// bar containing cpu usage, sample-rate, etc...
		addAndMakeVisible(mediaBar = new MediaBar(transportSource));	// pause/play/stop buttons, spectrogram toggle button
		addAndMakeVisible(menuBar = new MenuBarComponent(this));		// Options menus

		// menu bar setup
		commandManager.registerAllCommandsForTarget(this);
		menuBar->toFront(true);

		// update mediaBar with current playback state
		mediaBar->setPlaybackState(state);
		mediaBar->addButtonListeners(this);
		mediaBar->addSliderListeners(this);

		// optional at the moment - may remove
		addAndMakeVisible(spectrogram = new SpectrogramComponent());
    }

    ~MainContentComponent()
    {
        shutdownAudio();
		reader = nullptr;
    }

    //=======================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		currentBufferSize = samplesPerBlockExpected;
		currentSampleRate = sampleRate;

		// call prepareToPlay on relevant Sources
		transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

		highPassFilterSource = new HighPassFilterAudioSource(&transportSource);
		highPassFilterSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
		if (readerSource == nullptr)
		{
			bufferToFill.clearActiveBufferRegion();
			return;
		}

		//transportSource.getNextAudioBlock(bufferToFill);
		highPassFilterSource->getNextAudioBlock(bufferToFill);
		spectrogram->getNextAudioBlock(bufferToFill);
    }

    void releaseResources() override
    {
		transportSource.releaseResources();
		transportSource.setSource(nullptr);
    }

    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::whitesmoke);

        // get each components position relative to window size
		Rectangle<int> localBounds = getLocalBounds();
		Rectangle<int> menuBarArea = localBounds.removeFromTop(LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight());
		Rectangle<int> infoBarArea = localBounds.removeFromBottom(30);
		Rectangle<int> mediaBarArea = localBounds.removeFromTop(localBounds.getHeight() / 2);
		Rectangle<int> spectrogramArea = localBounds;
		
		// set bounds for each component in window
		menuBar->setBounds(menuBarArea);
		mediaBar->setBounds(mediaBarArea);
		infoBar->setBounds(infoBarArea);
		spectrogram->setBounds(spectrogramArea.reduced(5));

		// update InfoBar labels with current CPU usage, sample rate and buffer size
		infoBar->updateCpu(deviceManager.getCpuUsage() * 100);
		infoBar->updateBufferSize(currentBufferSize);
		infoBar->updateSampleRate(currentSampleRate);

		// update mediaBar with current playback state
		mediaBar->setPlaybackState(state);
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }

	// methods inherited from MenuBarModel
	StringArray getMenuBarNames()
	{
		StringArray names = { "File", "Options", "Help" };
		return names;
	}

	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName)
	{
		PopupMenu menu;

		if (topLevelMenuIndex == 0)
		{
			// File menu
			menu.addCommandItem(&commandManager, LOAD);
			menu.addCommandItem(&commandManager, QUIT);
		}
		else if (topLevelMenuIndex == 1)
		{
			// Options menu
			menu.addCommandItem(&commandManager, PLAY);
			menu.addCommandItem(&commandManager, STOP);
			menu.addSeparator();
			menu.addCommandItem(&commandManager, SETTINGS);
		}
		else if (topLevelMenuIndex == 2)
		{
			// Help menu
			menu.addCommandItem(&commandManager, ABOUT);
		}

		return menu;
	}

	void menuItemSelected(int menuItemID, int topLevelMenuIndex)
	{
	}
	// ===================================

	// methods inherited from ApplicationCommandTarget
	ApplicationCommandTarget* getNextCommandTarget()
	{
		return findFirstTargetParentComponent();
	}

	void getAllCommands(Array<CommandID> &commands)
	{
		const CommandID ids[] = { LOAD, QUIT, PLAY, STOP, SETTINGS, ABOUT };
		commands.addArray(ids, numElementsInArray(ids));
	}

	void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result)
	{
		const String menu("Menu");

		switch (commandID)
		{
		case CommandIDs::LOAD:
			result.setInfo("Load", "Pick a file to load into DrumBooth", menu, 0);
			result.addDefaultKeypress('L', ModifierKeys::commandModifier);
			break;

		case CommandIDs::QUIT:
			result.setInfo("Quit", "Exits DrumBooth", menu, 0);
			result.addDefaultKeypress('Q', ModifierKeys::commandModifier);
			break;

		case CommandIDs::PLAY:
			result.setInfo("Play", "Starts playback", menu, 0);
			result.addDefaultKeypress(KeyPress::spaceKey, ModifierKeys::noModifiers);
			break;

		case CommandIDs::STOP:
			result.setInfo("Stop", "Stops playback", menu, 0);
			result.addDefaultKeypress(KeyPress::spaceKey, ModifierKeys::commandModifier);
			break;

		case CommandIDs::SETTINGS:
			result.setInfo("Settings", "Open settings menu", menu, 0);
			result.addDefaultKeypress('Q', ModifierKeys::commandModifier);
			break;

		case CommandIDs::ABOUT:
			result.setInfo("About", "Information about this application", menu, 0);
		}
	}

	bool perform(const InvocationInfo& info)
	{
		switch (info.commandID)
		{
		case CommandIDs::LOAD:                                        
			loadButtonPressed();
			break;

		case CommandIDs::QUIT:
			quitButtonPressed();
			break;

		case CommandIDs::PLAY:
			playButtonPressed();
			break;

		case CommandIDs::STOP:
			stopButtonPressed();
			break;

		case CommandIDs::SETTINGS:
			settingsButtonPressed();
			break;

		case CommandIDs::ABOUT:
			aboutButtonPressed();
			break;
		default:
			return false;
		}

		return true;
	}

	// Not sure if i need this for keyboard shortcuts	
	bool keyPressed(const KeyPress& key) override
	{
		if (key == KeyPress::spaceKey)
		{
			playButtonPressed();
		}

		return true;
	}

	// ===============================================

	// methods inherited from ChangeListener
	void changeListenerCallback(ChangeBroadcaster* source)
	{
		if (source == &transportSource)
		{
			if (transportSource.isPlaying())
			{
				changeState(Playing);
			}
			else if ((state == Stopping))
			{
				changeState(Stopped);
			}
			else if (state == Pausing)
			{
				changeState(Paused);
			}

		}
	}
	// =====================================

	// methods inherited from Button::Listener
	void buttonClicked(Button* buttonThatWasClicked)
	{
		if (buttonThatWasClicked == mediaBar->button_LoadFile)
		{
			loadButtonPressed();
		}
		else if (buttonThatWasClicked == mediaBar->button_PlayPause)
		{
			playButtonPressed();
		}
		else if (buttonThatWasClicked == mediaBar->button_Stop)
		{
			stopButtonPressed();
		}
		else if (buttonThatWasClicked == mediaBar->button_spectrogramEnabled)
		{
			spectrogram->setEnabled(mediaBar->button_spectrogramEnabled->getToggleState());
		}
	}
	// =======================================

	// method inherited from Slider::Listener
	void sliderValueChanged(Slider* sliderThatWasChanged)
	{
		if (sliderThatWasChanged == mediaBar->slider_HighPassFilterFreq)
		{
			int newFreq = mediaBar->slider_HighPassFilterFreq->getValue();
			highPassFilterSource->setHighPassFilterFrequency(newFreq);
		}
		else if (sliderThatWasChanged == mediaBar->slider_LowPassFilterFreq)
		{
			int newFreq = mediaBar->slider_LowPassFilterFreq->getValue();
			//lowPassFilterSource->setLowPassFilterFrequency(newFreq);
		}

	}
	// =======================================
	
	


private:
    //==============================================================================
	enum CommandIDs
	{
		LOAD = 0x2001,
		SETTINGS = 0x2002,
		PLAY = 0x2003,
		STOP = 0x2004,
		ABOUT = 0x2005,
		QUIT = StandardApplicationCommandIDs::quit
	};

	// holds current transportSource state
	TransportState state;

	// handles changing transportSource state
	void changeState(TransportState newState)
	{
		if (state != newState && readerSource != nullptr)
		{
			state = newState;

			switch (state)
			{
			case Stopped:
				transportSource.setPosition(0.0);
				//spectrogram->setEnabled(false);
				break;

			case Starting:
				transportSource.start();
				spectrogram->setEnabled(true);
				break;

			case Playing:
				break;

			case Pausing:
				transportSource.stop();
				spectrogram->setEnabled(false);
				break;

			case Paused:
				break;

			case Stopping:
				transportSource.stop();
				spectrogram->setEnabled(false);
				break;
			}
		}
	}

	void loadButtonPressed()
	{

		FileChooser chooser("Select a file to load...",
			File::nonexistent,
			"*.wav; *.flac; *.mp3");
		if (chooser.browseForFileToOpen())
		{
			changeState(Stopping);
			File file(chooser.getResult());
			reader = nullptr;
			reader = formatManager.createReaderFor(file);

			if (reader != nullptr)
			{
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, false);
				transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                                                                                         
				readerSource = newSource.release();

			}

			spectrogram->setToClear(true);
		}
	}

	void playButtonPressed()
	{
		if (state == Stopped || state == Paused)
			changeState(Starting);
		else
			changeState(Pausing);
	}

	void stopButtonPressed()
	{
		changeState(Stopping);
	}

	void settingsButtonPressed() 
	{
		// pause playback
		// changeState(Pausing);

		ScopedPointer<AudioDeviceSelectorComponent> deviceSelector = new AudioDeviceSelectorComponent(deviceManager,
			0, 0, 2, 2,
			false,
			false,
			true,
			false);

		deviceSelector->setSize(500, 180);
		DialogWindow::showModalDialog(String("Audio Settings"),
			deviceSelector,
			TopLevelWindow::getTopLevelWindow(0),
			Colours::whitesmoke,
			true);
	}

	void quitButtonPressed()
	{
		JUCEApplication::getInstance()->systemRequestedQuit();
	}

	void aboutButtonPressed()
	{
		ScopedPointer<AboutPage> aboutPage = new AboutPage();
		aboutPage->setSize(300, 100);
		DialogWindow::showModalDialog("About DrumBooth",
			aboutPage,
			TopLevelWindow::getTopLevelWindow(0),
			Colours::whitesmoke,
			true);
	}

	// Audio
	double currentSampleRate;
	int currentBufferSize;
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReader> reader;

	// AudioSources
	AudioTransportSource transportSource;
	ScopedPointer<AudioFormatReader> formatReader;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	ScopedPointer<Separator> separationSource;
	ScopedPointer<LowPassFilterAudioSource> lowPassFilterSource;
	ScopedPointer<HighPassFilterAudioSource> highPassFilterSource;
	
	// Application Components
	ScopedPointer<InfoBar> infoBar;
	ScopedPointer<MediaBar> mediaBar;
	ScopedPointer<MenuBarComponent> menuBar;
	ScopedPointer<SpectrogramComponent> spectrogram;
	ApplicationCommandManager commandManager;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
