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

class RudimentBrowser : public Component,
						public FileBrowserListener
						/*
						,	public Button::Listener,
							public Slider::Listener
						*/
{
public:

	RudimentBrowser();
	~RudimentBrowser();

	void paint(Graphics& g) override;
	void resized() override;

	void loadImages();

private:
	// heap variables go here
	WildcardFileFilter wildcardFileFilter;
	TimeSliceThread imageScanThread;
	DirectoryContentsList imageList;
	FileTreeComponent fileTree;

	ImageComponent imagePreview;

	void selectionChanged() override
	{
		// we're only really interested in when the selection changes, regardless of if it was
		// clicked or not so we'll only override this method
		const File selectedFile(fileTree.getSelectedFile());

		if (selectedFile.existsAsFile())
			imagePreview.setImage(ImageCache::getFromFile(selectedFile));

		// the image cahce is a handly way to load images from files or directly from memory and
		// will keep them hanging around for a few seconds in case they are requested elsewhere
	}

	void fileClicked(const File&, const MouseEvent&) override {}
	void fileDoubleClicked(const File&) override {}
	void browserRootChanged(const File&) override {}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RudimentBrowser)
};



#endif  // RUDIMENTBROWSER_H_INCLUDED
