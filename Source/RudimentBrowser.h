/*
  ==============================================================================

    RudimentBrowser.h
    Created: 1 Mar 2016 4:12:09pm
    Author:  Sean

  ==============================================================================
*/

#ifndef RUDIMENTBROWSER_H_INCLUDED
#define RUDIMENTBROWSER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class RudimentBrowser : public Component
{
public:

	RudimentBrowser();
	~RudimentBrowser();

	void paint(Graphics& g) override;
	void resized() override;

private:
	// heap variables go here

};



#endif  // RUDIMENTBROWSER_H_INCLUDED
