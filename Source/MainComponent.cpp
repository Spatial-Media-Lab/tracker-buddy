#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : midiDeviceManagerComponent (midiDeviceManager)
{
    addAndMakeVisible (midiDeviceManagerComponent);

    addAndMakeVisible (deviceEditor);
    deviceEditor.onReturnKey = [&] () { createDeviceInterface(); };

    setSize (600, 400);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced (20);
    auto row = bounds.removeFromTop (200);

    midiDeviceManagerComponent.setBounds (row.removeFromLeft (400));
    row.removeFromLeft(20);
    deviceEditor.setBounds (row.removeFromBottom (30));

    if (trackerComponent)
    {
        bounds.removeFromTop (20);
        trackerComponent->setBounds (bounds);
    }
}


void MainComponent::createDeviceInterface()
{

    const auto deviceName = deviceEditor.getText();
    const auto pair = midiDeviceManager.getPair (deviceName);


    tracker = std::make_unique<TrackerInterface> (midiDeviceManager, pair);
    trackerComponent = std::make_unique<TrackerInterfaceComponent> (*tracker);

    tracker->openMidiDevices();
    addAndMakeVisible (*trackerComponent);

    resized();
}
