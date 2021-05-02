#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "tracker/Connection.h"
#include "tracker/MidiDeviceManager.h"
#include "tracker/TrackerInterface.h"
#include "tracker/TrackerInterfaceComponent.h"

#include "destinations/Destination.h"
#include "destinations/Visualizer.h"


class MainComponent   : public juce::Component
{
public:
    //==============================================================================
    MainComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    //==============================================================================
    MidiDeviceManager midiDeviceManager;

    std::list<std::unique_ptr<Connection>> connections;
    std::list<std::unique_ptr<ConnectionComponent>> components;

    juce::TextButton addDeviceButton;

    void showDeviceList();
    void createDeviceInterface (InputOutputPair pair);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
