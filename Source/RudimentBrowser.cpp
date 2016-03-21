/*
  ==============================================================================

    RudimentBrowser.cpp
    Created: 1 Mar 2016 4:12:09pm
    Author:  Sean

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
	g.fillAll(Colours::slategrey);
}

void RudimentBrowser::resized()
{
	Rectangle<int> localBounds = getLocalBounds().reduced(4);
	int width = getWidth();

	Rectangle<int> treeArea = localBounds.removeFromLeft(width / 3);
	fileTree.setBounds(treeArea);
	imagePreview.setBounds(localBounds);
}
