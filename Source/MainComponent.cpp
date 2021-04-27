#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : midiDeviceManagerComponent (midiDeviceManager)
{

    addAndMakeVisible (midiDeviceManagerComponent);

    setSize (600, 400);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    midiDeviceManagerComponent.setBounds (20, 20, 400, 250);
}
