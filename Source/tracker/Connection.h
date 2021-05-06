
#pragma once
#include <JuceHeader.h>
#include <list>
#include "../destinations/Destination.h"

#include "TrackerInterfaceComponent.h"

struct Connection
{
    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}
        virtual void destinationAdded (Destination& destination) {}
        virtual void destinationRemoved (const Destination& destination) {}
    };


    Connection (MidiDeviceManager& m, InputOutputPair device) :
    source (m, device)
    {
        source.openMidiDevices();
    }

    void addListener (Listener* newListener) { listeners.add (newListener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }


    void addDestination (std::unique_ptr<Destination> newDestination)
    {
        if (! newDestination)
            return;

        listeners.call ([&] (Listener& l) { l.destinationAdded (*newDestination); } );
        destinations.push_back (std::move (newDestination));
    }

    TrackerInterface& getSource()
    {
        return source;
    }

private:
    TrackerInterface source;
    std::list<std::unique_ptr<Destination>> destinations;

    juce::ListenerList<Listener> listeners;
};

