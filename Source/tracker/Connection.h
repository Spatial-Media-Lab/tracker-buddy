
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


struct ConnectionComponent : public juce::Component, public Connection::Listener
{
    ConnectionComponent (Connection& c) : connection (c), sourceComponent (c.getSource())
    {
        addAndMakeVisible (sourceComponent);

        addDestinationButton.setButtonText ("Add Destination...");
        addDestinationButton.onClick = [&] () { createDestinationPopup(); };

        addAndMakeVisible (addDestinationButton);

        connection.addListener (this);

        setSize (400, 80);
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
            connection.addDestination (DestinationFactory::createDestination (connection.getSource(), list[result - 1]));
        }
    }

    void destinationAdded (Destination& destination) override
    {
        auto component = destination.createWidget();
        auto* rawPtr = component.get();
        destinationComponents.push_back (std::move (component));
        addAndMakeVisible (rawPtr);
        resized();
    }

    void paint (juce::Graphics& g) override
    {

    }

    void childrenChanged() override
    {
        setSize (getWidth(), getTotalHeight());
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        if (auto target = getTotalHeight(); bounds.getHeight() != target)
        {
            setSize (getWidth(), target);
            return;
        }


        auto half = bounds.getWidth() / 2;

        sourceComponent.setBounds (bounds.removeFromLeft (half).removeFromTop (sourceComponent.getIdealHeight()));

        for (auto& c : destinationComponents)
        {
            c->setBounds (bounds.removeFromTop (c->getWidgetHeight()));
            bounds.removeFromTop (4);
        }

        addDestinationButton.setBounds (bounds.removeFromTop (destinationComponents.empty() ? 30 : addButtonHeightSmall));
    }


    int getTotalHeight() const
    {
        int sum = 0;
        for (auto& c : destinationComponents)
            sum += c->getWidgetHeight() + 4;

        sum += addButtonHeightSmall;

        return std::max (sourceComponent.getIdealHeight(), sum);
    }

private:
    const int addButtonHeightSmall = 15;

    TrackerInterfaceComponent sourceComponent;
    std::list<std::unique_ptr<Destination::Widget>> destinationComponents;
    Connection& connection;

    juce::TextButton addDestinationButton;
};
