/*
  ==============================================================================

    TODO:
		- Rudiment Browser Component
		- Implement Harmonic/Percussive Separator Audio Source
		- Investigate AudioProcessor Graph as alternative for AudioSources

		- line 309: adjust screen size when spectrogram disabled	

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
		//mediaBar->addSliderListeners(this);

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
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
		if (readerSource == nullptr)
		{
			bufferToFill.clearActiveBufferRegion();
			return;
		}

		transportSource.getNextAudioBlock(bufferToFill);
		//separationSource->getNextAudioBlock(bufferToFill);
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
		Rectangle<int> mediaBarArea = localBounds.removeFromTop(localBounds.getHeight()/2);
		Rectangle<int> spectrogramArea = localBounds;
		
		// set bounds for each component in window
		menuBar->setBounds(menuBarArea);
		mediaBar->setBounds(mediaBarArea);
		infoBar->setBounds(infoBarArea);
		spectrogram->setBounds(spectrogramArea.reduced(2));

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
			menu.addCommandItem(&commandManager, RUDIMENTS);
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
		const CommandID ids[] = { LOAD, QUIT, PLAY, STOP, SETTINGS, RUDIMENTS, ABOUT };
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

		case CommandIDs::RUDIMENTS:
			result.setInfo("Rudiment Browser", "Opens Rudiment Browser", menu, 0);
			result.addDefaultKeypress(KeyPress::F2Key, ModifierKeys::noModifiers);
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

		case CommandIDs::RUDIMENTS:
			rudimentBrowserTriggered();
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
		else if (key == KeyPress::F2Key)
		{
			rudimentBrowserTriggered();
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
			// TODO: adjust screen size here to hide spectrogram when not in use
			bool enable = mediaBar->button_spectrogramEnabled->getToggleState();

			spectrogram->setEnabled(enable);
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
		RUDIMENTS = 0x2006,
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
		// Open file chooser dialog
		FileChooser chooser("Select a file to load...",
			File::nonexistent,
			"*.wav; *.flac; *.mp3");

		// if user selects a file
		if (chooser.browseForFileToOpen())
		{
			//stop playback
			stopButtonPressed();

			// get file and create reader
			File file(chooser.getResult());
			reader = nullptr;
			reader = formatManager.createReaderFor(file);
			
			// display track title in player
			mediaBar->setTrackInfo(file.getFileNameWithoutExtension());

			if (reader != nullptr)
			{
				// add new source to transport, remove old source
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, false);
				transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                                                                                         
				readerSource = newSource.release();
			}
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
		// initialise device selector component
		AudioDeviceSelectorComponent* deviceSelector = new AudioDeviceSelectorComponent(deviceManager,
			0, 0, 2, 2,
			false,
			false,
			true,
			true);

		deviceSelector->setSize(500, 180);		// set component size

		// initialise LaunchOptions struct with relevant fields
		DialogWindow::LaunchOptions options;
		options.dialogTitle = "Audio Settings";
		options.useNativeTitleBar = true;
		options.dialogBackgroundColour = Colours::whitesmoke;
		options.escapeKeyTriggersCloseButton = true;
		options.useBottomRightCornerResizer = false;
		options.resizable = false;
		options.content = OptionalScopedPointer<Component>(deviceSelector, true);
		options.launchAsync(); // launch dialog
	}

	void quitButtonPressed()
	{
		JUCEApplication::getInstance()->systemRequestedQuit();
	}

	void aboutButtonPressed()
	{
		// initialise About component
		AboutPage* aboutPage = new AboutPage();
		
		// create LaunchOptions struct
		DialogWindow::LaunchOptions options;
		options.dialogTitle = "About DrumBooth";
		options.useNativeTitleBar = true;
		options.escapeKeyTriggersCloseButton = true;
		options.useBottomRightCornerResizer = false;
		options.resizable = false;
		options.content = OptionalScopedPointer<Component>(aboutPage, true);
		options.launchAsync(); // launch dialog
	}

	void rudimentBrowserTriggered()
	{		
		RudimentBrowser* rudimentBrowser = new RudimentBrowser();

		DialogWindow::LaunchOptions options;
		options.dialogTitle = "Rudiment Browser";
		options.useNativeTitleBar = true;
		options.escapeKeyTriggersCloseButton = true;
		options.useBottomRightCornerResizer = true; // needs to be resizable, exercise images need more room than rudiments
		options.resizable = true;
		options.content = OptionalScopedPointer<Component>(rudimentBrowser, true);
		options.launchAsync();
	}

	Rectangle<int> spectrogramArea;

	// Audio
	double currentSampleRate;
	int currentBufferSize;
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReader> reader;

	// AudioSources
	AudioTransportSource transportSource;
	ScopedPointer<MixerAudioSource> mixerSource;
	ScopedPointer<AudioFormatReader> formatReader;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	//ScopedPointer<Separator> separationSource;
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
