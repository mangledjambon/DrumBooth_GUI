/*
  ==============================================================================

    AboutPage.h
    Created: 19 Feb 2016 1:08:20pm
    Author:  Sean

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#ifndef ABOUTPAGE_H_INCLUDED
#define ABOUTPAGE_H_INCLUDED

class AboutPage : public Component
{
public:
	AboutPage();
	~AboutPage();

	void paint(Graphics& g) override;
	void resized() override;

private:
	Image logo;
	ImageComponent imageComponent;

};



#endif  // ABOUTPAGE_H_INCLUDED
