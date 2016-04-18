/*
  ==============================================================================

    RudimentBrowser.cpp
    Created: 1 Mar 2016 4:12:09pm
    Author:  Sean

	TODO:
		1. fix crash when resizing to small width -> Possible fix: set minimum width of component
  ==============================================================================
*/

#include "RudimentBrowser.h"

RudimentBrowser::RudimentBrowser()
	:	wildcardFileFilter("*.jpeg;*.jpg;*.gif", "*", "Rudiment Images"),
		imageScanThread("Images Scanner Thread"),
		imageList(&wildcardFileFilter, imageScanThread),
		fileTree(imageList)
{
	setSize(700, 300);
	setOpaque(true);
	imageList.setDirectory(File::getCurrentWorkingDirectory().getChildFile("../../Source/rudiments"), true, true);
	imageScanThread.startThread(1);
	fileTree.addListener(this);
	fileTree.setColour(TreeView::backgroundColourId, Colours::lightgrey);

	imageTitleLabel.setJustificationType(Justification::centred);
	imageTitleLabel.setText(TRANS("Select a rudiment from the drop-down list and it will be displayed below."), dontSendNotification);
	addAndMakeVisible(imageTitleLabel);
	addAndMakeVisible(fileTree);
	addAndMakeVisible(imagePreview);
}

RudimentBrowser::~RudimentBrowser()
{
	if (imageScanThread.isThreadRunning())
		imageScanThread.signalThreadShouldExit();

	fileTree.removeListener(this);
}

void RudimentBrowser::paint(Graphics& g)
{
	g.fillAll(Colours::whitesmoke);

	Rectangle<int> localBounds = getLocalBounds().reduced(4);
	int width = getWidth();

	Rectangle<int> treeArea = localBounds.removeFromLeft(width / 3);
	Rectangle<int> labelArea = localBounds.removeFromTop(40).reduced(4);

	g.setColour(Colours::darkslategrey);
	g.drawRect(labelArea);
	fileTree.setBounds(treeArea);
	imageTitleLabel.setBounds(labelArea);
	imagePreview.setBounds(localBounds.reduced(5));
}

void RudimentBrowser::resized()
{
	
}
