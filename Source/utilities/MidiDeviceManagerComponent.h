
#pragma once

#include <JuceHeader.h>
#include "MidiDeviceManager.h"


class MidiDeviceManagerComponent : public juce::Component, public MidiDeviceManager::Listener
{
public:
    MidiDeviceManagerComponent (MidiDeviceManager& managerToConnectTo)
    : midiDeviceManager (managerToConnectTo)
    {
        midiDeviceManager.addListener (this);

        editor.setMultiLine (true);
        editor.setReadOnly (true);
        addAndMakeVisible (editor);
    }

    ~MidiDeviceManagerComponent() override
    {
        midiDeviceManager.removeListener (this);
    }


    void devicesChanged (const juce::Array<InputOutputPair>& currentList) override
    {
        devices = currentList;

        editor.clear();

        editor.insertTextAtCaret ("INPUTS\n");
        for (auto& dev : devices)
        {
            editor.insertTextAtCaret (dev.inputDevice.name << ": " << dev.inputDevice.identifier);
            editor.insertTextAtCaret ("\n");
        }

        editor.insertTextAtCaret ("\n");

        editor.insertTextAtCaret ("OUTPUTS\n");
        for (auto& dev : devices)
        {
            editor.insertTextAtCaret (dev.outputDevice.name << ": " << dev.outputDevice.identifier);
            editor.insertTextAtCaret ("\n");
        }
    }


    void resized() override
    {
        editor.setBounds (getLocalBounds());
    }

private:
    MidiDeviceManager& midiDeviceManager;
    juce::Array<InputOutputPair> devices;

    juce::TextEditor editor;
};
