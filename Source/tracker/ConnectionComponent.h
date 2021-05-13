
#pragma once
#include <JuceHeader.h>

#include "Connection.h"
#include "ConnectionManager.h"

#include "TrackerInterfaceComponent.h"
#include "AddButton.h"
#include "RemoveButton.h"


struct ConnectionComponent : public juce::Component, public Connection::Listener
{
    static constexpr int leftWidth = 200;
    static constexpr int rightWidth = 400;

    ConnectionComponent (Connection& c, ConnectionManager& cm) : connection (c), connectionManager (cm), sourceComponent (c.getSource())
    {
        addAndMakeVisible (sourceComponent);

        addDestinationButton.onClick = [&] () { createDestinationPopup(); };
        addAndMakeVisible (addDestinationButton);

        removeConnectionButton.onClick = [&] () { connectionManager.removeConnection (&c); };
        addAndMakeVisible (removeConnectionButton);

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
            connection.addDestination (DestinationFactory::createDestination (connection.getSource(), list[result - 1]));
        }
    }

    void destinationAdded (Destination& destination) override
    {
        auto wrapper = std::make_unique<WidgetWrapper> (destination.createWidget());
        wrapper->removeButton.onClick = [&] () { connection.removeDestination (&destination); };
        auto* rawPtr = wrapper.get();
        destinationComponents.push_back (std::move (wrapper));
        addAndMakeVisible (rawPtr->widget.get());
        addAndMakeVisible (rawPtr->removeButton);
        resized();
        repaint();
    }

    void destinationAboutToBeRemoved (const Destination& destination) override
    {
        for (auto& c : destinationComponents)
            if (&c->widget->getDestination() == &destination)
            {
                destinationComponents.remove (c);
                break;
            }
        resized();
        repaint();
    }

    void childBoundsChanged (Component* child) override
    {
        resized();
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::white.withAlpha (0.5f));

        const auto start = addDestinationButton.getBoundsInParent().toFloat().getCentre();

        for (auto& c : destinationComponents)
        {
            juce::Path line;
            const auto end = c->removeButton.getBoundsInParent().toFloat().getCentre();
            const auto center = (start + end) / 2;

            const auto controlA = juce::Point<float> ((start.getX() + center.getX()) / 2, start.getY());
            const auto controlB = juce::Point<float> ((end.getX() + center.getX()) / 2, end.getY());

            line.startNewSubPath (start);
            line.quadraticTo (controlA, center);
            line.quadraticTo (controlB, end);
            g.strokePath (line, juce::PathStrokeType (2.0f));
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        if (auto target = getTotalHeight(); bounds.getHeight() != target)
        {
            setSize (getWidth(), target);
            return;
        }

        auto leftCol = bounds.removeFromLeft (leftWidth);
        auto rightCol = bounds.removeFromRight (rightWidth);

        sourceComponent.setBounds (leftCol.removeFromTop (sourceComponent.getIdealHeight()));
        addDestinationButton.setBounds (bounds.removeFromTop (20).removeFromLeft (20));

        bounds.removeFromTop (3);

        removeConnectionButton.setBounds (bounds.removeFromTop (20).removeFromLeft (20));

        for (auto& c : destinationComponents)
        {
            auto row = rightCol.removeFromTop (c->widget->getWidgetHeight());
            c->removeButton.setBounds (row.removeFromLeft (20).removeFromTop (20));
            c->widget->setBounds (row);
            rightCol.removeFromTop (4);
        }
    }

    Connection& getConnection()
    {
        return connection;
    }


    int getTotalHeight() const
    {
        int sum = 0;
        for (auto& c : destinationComponents)
            sum += c->widget->getWidgetHeight() + 4;

        return std::max (sourceComponent.getIdealHeight(), sum);
    }

private:
    struct WidgetWrapper
    {
        WidgetWrapper (std::unique_ptr<Destination::Widget> w) : widget (std::move (w))
        {}

        std::unique_ptr<Destination::Widget> widget;
        RemoveButton removeButton;
    };

    ConnectionManager& connectionManager;
    Connection& connection;

    TrackerInterfaceComponent sourceComponent;
    std::list<std::unique_ptr<WidgetWrapper>> destinationComponents;


    AddButton addDestinationButton;
    RemoveButton removeConnectionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectionComponent)
};

