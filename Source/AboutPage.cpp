/*
  ==============================================================================

    AboutPage.cpp
    Created: 19 Feb 2016 1:08:20pm
    Author:  Sean

  ==============================================================================
*/

#include "AboutPage.h"

AboutPage::AboutPage()
{}

AboutPage::~AboutPage()
{}

void AboutPage::paint(Graphics& g)
{
	g.fillAll(Colours::whitesmoke);   // fill the component with a colour

	g.setColour(Colours::darkslategrey);
	g.setFont(14.0f);
	g.drawFittedText(JUCEApplication::getInstance()->getApplicationName()
		+ " v" + JUCEApplication::getInstance()->getApplicationVersion()
		+ "\nSean Breen, R00070693\nDCOM4 Final Year Project 2016\nCork Institute of Technology", getLocalBounds(),
		Justification::centred, true);
}

void AboutPage::resized()
{}