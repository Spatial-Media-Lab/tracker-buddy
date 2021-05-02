#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "tracker/MidiDeviceManager.h"
#include "tracker/MidiDeviceManagerComponent.h"
#include "tracker/TrackerInterface.h"
#include "tracker/TrackerInterfaceComponent.h"

#include "destinations/Destination.h"
#include "destinations/Visualizer.h"

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
    {}

    TrackerInterface source;


    void addListener (Listener* newListener) { listeners.add (newListener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }


    void addDestination (std::unique_ptr<Destination> newDestination)
    {
        listeners.call ([&] (Listener& l) { l.destinationAdded (*newDestination); } );
        destinations.push_back (std::move (newDestination));
    }

private:
    juce::ListenerList<Listener> listeners;
    std::list<std::unique_ptr<Destination>> destinations;
};


struct ConnectionComponent : public juce::Component, public Connection::Listener
{
    ConnectionComponent (Connection& c) : connection (c), sourceComponent (c.source)
    {
        addAndMakeVisible (sourceComponent);

        addDestinationButton.setButtonText ("Add Destination...");
        addDestinationButton.onClick = [&] ()
        {
            connection.addDestination (std::make_unique<Visualizer> (connection.source));
        };

        addAndMakeVisible (addDestinationButton);


        connection.addListener (this);
    }


    ~ConnectionComponent() override
    {
        connection.removeListener (this);
    }


    void destinationAdded (Destination& destination) override
    {
        auto component = destination.createComponent();
        addAndMakeVisible (component.get());
        destinationComponents.push_back (std::move (component));
        resized();
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto half = bounds.getWidth() / 2;

        sourceComponent.setBounds (bounds.removeFromLeft (half));

        if (destinationComponents.empty())
        {
            addDestinationButton.setVisible (true);
            addDestinationButton.setBounds (bounds);
        }
        else
        {
            addDestinationButton.setVisible (false);
            for (auto& c : destinationComponents)
                c->setBounds (bounds);
        }
    }

private:
    TrackerInterfaceComponent sourceComponent;
    std::list<std::unique_ptr<juce::Component>> destinationComponents;
    Connection& connection;

    juce::TextButton addDestinationButton;
};


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

    std::list<std::unique_ptr<Connection>> connections;
    std::list<std::unique_ptr<ConnectionComponent>> components;


    juce::TextEditor deviceEditor;


    void createDeviceInterface();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
