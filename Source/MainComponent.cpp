#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : connectionList (connectionManager)
{
    viewPort.setViewedComponent (&connectionList, false);
    addAndMakeVisible (viewPort);

    addDeviceButton.setButtonText ("Add device...");
    addDeviceButton.onClick = [&] () { showDeviceList(); };
    addAndMakeVisible (addDeviceButton);

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

    addDeviceButton.setBounds (bounds.removeFromTop (40));

    bounds.removeFromTop (10);

    viewPort.setBounds (bounds);
}

void MainComponent::showDeviceList()
{
    auto devices = midiDeviceManager.getAvailableDevices();

    juce::PopupMenu m;
    m.addSectionHeader ("Available devices");
    for (int i = 0; i < devices.size(); ++i)
        m.addItem (i + 1, devices[i].inputDevice.name);

    const auto result = m.show();

    if (result > 0)
        createDeviceInterface (devices[result - 1]);
}


void MainComponent::createDeviceInterface (InputOutputPair pair)
{
    auto newConnection = std::make_unique<Connection> (midiDeviceManager, pair);

    connectionManager.addConnection (std::move (newConnection));

    resized();
}
