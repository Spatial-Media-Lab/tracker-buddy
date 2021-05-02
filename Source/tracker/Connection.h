
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
    {}

    TrackerInterface source;


    void addListener (Listener* newListener) { listeners.add (newListener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }


    void addDestination (std::unique_ptr<Destination> newDestination)
    {
        if (! newDestination)
            return;

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
        addDestinationButton.onClick = [&] () { createDestinationPopup(); };

        addAndMakeVisible (addDestinationButton);

        connection.addListener (this);
    }


    ~ConnectionComponent() override
    {
        connection.removeListener (this);
    }

    void createDestinationPopup()
    {
        auto list = DestinationFactory::getListOfDestinations();

        juce::PopupMenu m;
        m.addSectionHeader ("Choose destinations:");
        for (int i = 0; i < list.size(); ++i)
            m.addItem (i + 1, list[i]);

        const auto result = m.show();

        if (result > 0)
        {
            connection.addDestination (DestinationFactory::createDestination (connection.source, list[result - 1]));
        }
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

        sourceComponent.setBounds (bounds.removeFromLeft (half).removeFromTop (50));

        for (auto& c : destinationComponents)
        {
            c->setBounds (bounds.removeFromTop (30));
            bounds.removeFromTop (4);
        }

        addDestinationButton.setBounds (bounds.removeFromTop (destinationComponents.empty() ? 30 : 15));
    }

private:
    TrackerInterfaceComponent sourceComponent;
    std::list<std::unique_ptr<juce::Component>> destinationComponents;
    Connection& connection;

    juce::TextButton addDestinationButton;
};
