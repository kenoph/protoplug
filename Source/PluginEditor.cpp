/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "guiclasses/ProtoPopout.h"
#include "ProtoplugDir.h"


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4355) // 'this' used in base member initializer list
#endif
//==============================================================================
LuaProtoplugJuceAudioProcessorEditor::LuaProtoplugJuceAudioProcessorEditor (LuaProtoplugJuceAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
	content(this, ownerFilter),
	yank("bring to front"),
	popin("pop back in"),
	locateFiles("locate directory...")
{
	poppedWin = 0;
	processor = ownerFilter;
	ownerFilter->setProtoEditor(&content);
	addChildComponent(&yank);
	addChildComponent(&popin);
	addChildComponent(&locateFiles);
	yank.addListener(this);
	popin.addListener(this);
	locateFiles.addListener(this);
	//yank.setLookAndFeel(&laf);
	//popin.setLookAndFeel(&laf);
	yank.setBounds(20, 50, 150, 30);
	yank.setCentrePosition(140, 60);
	popin.setBounds(20, 95, 150, 30);
	popin.setCentrePosition(140, 102);
	locateFiles.setBounds(45, 95, 150, 30);
	locateFiles.setCentrePosition(190, 95);

	if (!ProtoplugDir::Instance()->found) {
		setSize (380, 130);
		locateFiles.setVisible(true);
	} else if (processor->popout) {
		content.initProtoplugDir();
		content.setSize(ownerFilter->lastUIWidth,
				        ownerFilter->lastUIHeight);
		popOut();
	} else {
		content.initProtoplugDir();
		addAndMakeVisible(&content);
		content.takeFocus();
		setSize (ownerFilter->lastUIWidth,
				 ownerFilter->lastUIHeight);
	}
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

LuaProtoplugJuceAudioProcessorEditor::~LuaProtoplugJuceAudioProcessorEditor()
{
	if (poppedWin!=0) {
		processor->lastPopoutX = poppedWin->getX();
		processor->lastPopoutY = poppedWin->getY();
	}
	content.saveCode();
}

void LuaProtoplugJuceAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll (Colours::white);
	if ( ! ProtoplugDir::Instance()->found)
	{
		g.setColour (Colours::black);
		g.setFont (15.0f);
		g.drawFittedText (
			"ProtoplugFiles not found! Please locate the \"ProtoplugFiles\" directory " 
			" (which should contain \"generators\", \"effects\", \"themes\"...)",
			10, 0, 360, 80,
			Justification::centred, 3);
	}
	else if (poppedWin)
	{
		g.setColour (Colours::black);
		g.setFont (15.0f);
		g.drawFittedText ("interface popped out",
						  0, 0, 280, 50,
						  Justification::centred, 1);
	}
}

void LuaProtoplugJuceAudioProcessorEditor::resized()
{
	if (poppedWin==0)
		content.setBounds(0, 0, getWidth(), getHeight());
}

void LuaProtoplugJuceAudioProcessorEditor::buttonClicked(Button *b)
{
	if (b==&yank && poppedWin)
		poppedWin->toFront(true);
	else if (b==&popin && poppedWin)
		popIn();
	else if (b==&locateFiles) {
		FileChooser fileOpen(
			"Where did you put my ProtoplugFiles directory:", 
			File::getSpecialLocation(File::currentApplicationFile).getParentDirectory());
		if (fileOpen.browseForDirectory())
		{
			File chosen = fileOpen.getResult();
			String missing;
			if (ProtoplugDir::Instance()->checkDir(chosen, missing)) {
				ProtoplugDir::Instance()->setDir(chosen);
				if (ProtoplugDir::Instance()->getDirTextFile().create().wasOk())
					ProtoplugDir::Instance()->getDirTextFile().replaceWithText(chosen.getFullPathName());
				setSize (670, 455);
				processor->luli->initProtoplugDir();
				locateFiles.setVisible(false);
				addAndMakeVisible(&content);
				content.takeFocus();
				content.initProtoplugDir();
			} else {
				AlertWindow::showNativeDialogBox("Protoplug", "Wrong directory: \"" + missing +
					"\" was not found in the given directory.", false);
			}
		}
	
	}
}

void LuaProtoplugJuceAudioProcessorEditor::popOut()
{
	poppedWin = new ProtoPopout(this, processor->getName(), Colours::white, DocumentWindow::allButtons, true);
	poppedWin->setAlwaysOnTop(processor->alwaysontop);
	poppedWin->setResizable(true, false);
	poppedWin->setUsingNativeTitleBar(true);
	poppedWin->setContentNonOwned(&content, true);
	processor->popout = true;
	poppedWin->setContentComponentSize(processor->lastUIWidth, processor->lastUIHeight);
	poppedWin->setTopLeftPosition(processor->lastPopoutX, processor->lastPopoutY);
	content.setPoppedOut(true);
	poppedWin->setVisible(true);
	setSize (280, 130);
	yank.setVisible(true);
	content.takeFocus();
	popin.setVisible(true);
}

void LuaProtoplugJuceAudioProcessorEditor::popIn()
{
	processor->lastUIWidth = 670;
	processor->lastUIHeight = 455;
	if (poppedWin!=0) {
		processor->lastPopoutX = poppedWin->getX();
		processor->lastPopoutY = poppedWin->getY();
	}
	processor->popout = false;
	int w=processor->lastUIWidth, h=processor->lastUIHeight;
	addAndMakeVisible(&content);
	content.setPoppedOut(false);
	setSize (w,h);
	content.setSize (w,h);
	poppedWin = 0;
	yank.setVisible(false);
	content.takeFocus();
	popin.setVisible(false);
	//content.resized();
}

void LuaProtoplugJuceAudioProcessorEditor::handleCommandMessage(int com)
{
	if (com==MSG_POPOUT && !processor->popout)
	{
		popOut();
	}
	else if (com==MSG_POPOUT && processor->popout)
	{
		popIn();
	}
	else if (com==MSG_ALWAYSONTOP)
	{
		processor->alwaysontop = !processor->alwaysontop;
		if (poppedWin!=0)
			poppedWin->setAlwaysOnTop(processor->alwaysontop);
	}
}
