
#pragma once
#include <JuceHeader.h>

#include "Connection.h"
#include "ConnectionManager.h"


struct ConnectionComponent : public juce::Component, public Connection::Listener
{
    ConnectionComponent (Connection& c, ConnectionManager& cm) : connection (c), connectionManager (cm), sourceComponent (c.getSource())
    {
        addAndMakeVisible (sourceComponent);

        addDestinationButton.setButtonText ("Add Destination...");
        addDestinationButton.onClick = [&] () { createDestinationPopup(); };
        addAndMakeVisible (addDestinationButton);

        removeConnectionButton.setButtonText ("Remove");
        removeConnectionButton.onClick = [&] () { connectionManager.removeConnection (&c); };
        addAndMakeVisible (removeConnectionButton);

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

        removeConnectionButton.setBounds (bounds.removeFromLeft (50).removeFromTop (20));

        for (auto& c : destinationComponents)
        {
            c->setBounds (bounds.removeFromTop (c->getWidgetHeight()));
            bounds.removeFromTop (4);
        }

        addDestinationButton.setBounds (bounds.removeFromTop (destinationComponents.empty() ? 30 : addButtonHeightSmall));
    }

    Connection& getConnection()
    {
        return connection;
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

    ConnectionManager& connectionManager;
    Connection& connection;

    TrackerInterfaceComponent sourceComponent;
    std::list<std::unique_ptr<Destination::Widget>> destinationComponents;


    juce::TextButton addDestinationButton;
    juce::TextButton removeConnectionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectionComponent)
};

