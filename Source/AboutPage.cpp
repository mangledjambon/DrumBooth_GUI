/*
  ==============================================================================

    AboutPage.cpp
    Created: 19 Feb 2016 1:08:20pm
    Author:  Sean

	TODO: 
		1. Update this page text
		2. Add CIT logo image

  ==============================================================================
*/

#include "AboutPage.h"

AboutPage::AboutPage()
{
	setSize(500, 400);

	logo = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("../../Source/res/logo.png"));
	imageComponent.setImage(logo);
	addAndMakeVisible(imageComponent);
}

AboutPage::~AboutPage()
{}

void AboutPage::paint(Graphics& g)
{
	// 0xd311645 = CIT red
	g.fillAll(Colour(211, 17, 69));   // fill the component with a colour

	Rectangle<int> localBounds = getLocalBounds();
	Rectangle<int> logoArea = localBounds.removeFromTop(getHeight()/2).reduced(5);
	imageComponent.setBounds(logoArea);

	Rectangle<int> textArea = localBounds;
	g.setColour(Colours::whitesmoke);
	g.fillRect(textArea.toFloat().reduced(5));
	g.setColour(Colours::darkslategrey);
	g.setFont(20.0f);
	g.drawFittedText(JUCEApplication::getInstance()->getApplicationName()
		+ " v" + JUCEApplication::getInstance()->getApplicationVersion() 
		+ "\nDeveloped by Sean Breen\nDCOM4 Final Year Project 2016",textArea.reduced(10), Justification::centredTop, 1);

	g.setFont(16.0f);
	g.drawFittedText("\nDrumBooth is a free, open-source software application for Windows which enables the user to isolate and separate the harmonic and percussive components from an audio file.\n\nSeparation algorithm adapted from Derry Fitzgerald's technique described in his paper 'Harmonic-Percussive Separation Using Median Filtering', DaFX10.", 
		textArea.removeFromBottom(128).reduced(10),
		Justification::horizontallyJustified, true);
}

void AboutPage::resized()
{}