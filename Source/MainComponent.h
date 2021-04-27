#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "utilities/MidiDeviceManager.h"
#include "utilities/MidiDeviceManagerComponent.h"
#include "utilities/TrackerInterface.h"
#include "utilities/TrackerInterfaceComponent.h"



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
    MidiDeviceManagerComponent midiDeviceManagerComponent;

    TrackerInterface tracker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
