
#pragma once

#include <JuceHeader.h>


struct InputOutputPair
{
    juce::MidiDeviceInfo inputDevice;
    juce::MidiDeviceInfo outputDevice;

    bool operator== (const InputOutputPair& other) const noexcept { return inputDevice == other.inputDevice && outputDevice == other.outputDevice; }
    bool operator!= (const InputOutputPair& other) const noexcept { return ! operator== (other); }
};


class MidiDeviceManager : public juce::Timer
{
public:

    struct Listener
    {
        virtual void devicesChanged (const juce::Array<InputOutputPair>& devices) = 0;
    };

    MidiDeviceManager()
    {
        startTimer (1000);
    }

    ~MidiDeviceManager() override = default;


    void timerCallback() override
    {
        updateDeviceList();
    }

    void updateDeviceList()
    {
        // TODO: on windows, device identifiers should be the same for input and output devices (of the same device). on macOS they are different (as juce just uses hash for <devicename>.<input/output> we might want to handle that differently!


        const auto inputs = juce::MidiInput::getAvailableDevices();
        const auto outputs = juce::MidiOutput::getAvailableDevices();

        juce::Array<InputOutputPair> newList;

        for (auto& inDev : inputs)
            for (auto& outDev : outputs)
                if (inDev.name == outDev.name)
                {
                    newList.add ({inDev, outDev});
                    break;
                }


        // check if devices have changed
        if (newList.size() != availableDevices.size())
        {
            swapLists (newList);
            return;
        }

        const int numDevices = newList.size();
        for (int d = 0; d < numDevices; ++d)
            if (newList[d] != availableDevices[d])
            {
                swapLists (newList);
                return;
            }
    }

    void addListener (Listener* listenerToAdd)
    {
        listeners.add (listenerToAdd);
    }

    void removeListener (Listener* listenerToAdd)
    {
        listeners.remove (listenerToAdd);
    }

    juce::Array<InputOutputPair> getAvailableDevices() const
    {
        return availableDevices;
    }


private:
    juce::Array<InputOutputPair> availableDevices;
    juce::ListenerList<Listener> listeners;

    void swapLists (juce::Array<InputOutputPair>& newList)
    {
        std::swap (availableDevices, newList);


        std::cout << "Available devices:" << std::endl;
        for (auto& d : availableDevices)
        {
            std::cout << "-"
                      << "\tINPUT " << d.inputDevice.name << " (" << d.inputDevice.identifier << ")\n"
                      << "\tOUTPUT " << d.outputDevice.name << " (" << d.outputDevice.identifier << ")" <<
                      std::endl;
        }

        std::cout << std::endl;


        // notify listeners
        listeners.call ([&] (Listener& l) { l.devicesChanged (availableDevices); } );
    }
};
