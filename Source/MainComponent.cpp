#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : midiDeviceManagerComponent (midiDeviceManager)
{
    addAndMakeVisible (midiDeviceManagerComponent);

    addAndMakeVisible (deviceEditor);
    deviceEditor.onReturnKey = [&] () { createDeviceInterface(); };

    setSize (800, 600);
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

    for (auto& t : components)
    {
        bounds.removeFromTop (10);
        t->setBounds (bounds.removeFromTop (40));
    }
}


void MainComponent::createDeviceInterface()
{
    const auto deviceName = deviceEditor.getText();
    const auto pair = midiDeviceManager.getPair (deviceName);


    auto newConnection = std::make_unique<Connection> (midiDeviceManager, pair);
    auto newComponent = std::make_unique<ConnectionComponent> (*newConnection);

    newConnection->source.openMidiDevices();
    addAndMakeVisible (newComponent.get());


    connections.push_back (std::move (newConnection));
    components.push_back (std::move (newComponent));
    resized();
}
