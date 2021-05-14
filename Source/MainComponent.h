#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "tracker/Connection.h"
#include "tracker/ConnectionComponent.h"
#include "tracker/ConnectionManager.h"
#include "tracker/MidiDeviceManager.h"
#include "tracker/TrackerInterface.h"
#include "tracker/TrackerInterfaceComponent.h"

#include "destinations/Destination.h"
#include "destinations/Visualizer.h"


class ConnectionList : public juce::Component, public ConnectionManager::Listener
{
    static constexpr int spacing = 10;
public:
    ConnectionList (ConnectionManager& manager) : connectionManager (manager)
    {
        connectionManager.addListener (this);
        auto& connections = connectionManager.getConnections();
        for (auto& c : connections)
            connectionAdded (*c);
    }

    ~ConnectionList()
    {
        connectionManager.removeListener (this);
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour ({180, 180, 200});
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 5.0f);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromLeft (spacing);
        bounds.removeFromBottom (spacing);

        for (auto& t : components)
        {
            bounds.removeFromTop (spacing);
            t->setBounds (bounds.removeFromTop (t->getHeight()));
        }
    }

    void childBoundsChanged (juce::Component* child) override
    {
        updateSize();
    }

    void parentSizeChanged() override
    {
        updateSize();
    }

    void connectionAdded (Connection& connection) override
    {
        auto newComponent = std::make_unique<ConnectionComponent> (connection, connectionManager);
        addAndMakeVisible (newComponent.get());
        components.push_back (std::move (newComponent));

        updateSize();
    }

    void connectionAboutToBeRemoved (Connection& connection) override
    {
        for (auto& c : components)
            if (&c->getConnection() == &connection)
            {
                components.remove (c);
                break;
            }
        
        updateSize();
    }


    void updateSize()
    {
        int height = spacing;
        for (auto& c : components)
            height += spacing + c->getTotalHeight();

        setSize (getParentWidth(), height);
        resized();
    }

private:
    ConnectionManager& connectionManager;
    std::list<std::unique_ptr<ConnectionComponent>> components;
};

class MainComponent  : public juce::Component
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
    ConnectionManager connectionManager;

    ConnectionList connectionList;
    juce::Viewport viewPort;


    juce::TextButton addDeviceButton;

    void showDeviceList();
    void createDeviceInterface (InputOutputPair pair);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
