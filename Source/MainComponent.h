#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "tracker/Connection.h"
#include "tracker/MidiDeviceManager.h"
#include "tracker/TrackerInterface.h"
#include "tracker/TrackerInterfaceComponent.h"

#include "destinations/Destination.h"
#include "destinations/Visualizer.h"


class ConnectionManager
{
public:
    struct Listener
    {
        virtual void connectionAdded (Connection& connection) {}
    };

    ConnectionManager() = default;

    void addListener (Listener* newListener) { listeners.add (newListener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }


    void addConnection (std::unique_ptr<Connection> connectionToAdd)
    {
        if (! connectionToAdd)
            return;

        auto* connection = connectionToAdd.get();
        connections.push_back (std::move (connectionToAdd));

        listeners.call ([&] (Listener& l) { l.connectionAdded (*connection); });
    }

    const std::list<std::unique_ptr<Connection>>& getConnections() const { return connections; };

private:
    juce::ListenerList<Listener> listeners;
    std::list<std::unique_ptr<Connection>> connections;
};


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
        g.fillAll (juce::Colours::cornflowerblue.withAlpha (0.1f));
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        for (auto& t : components)
        {
            bounds.removeFromTop (spacing);
            t->setBounds (bounds.removeFromTop (t->getHeight()));
        }
    }

    void parentSizeChanged() override
    {
        updateSize();
    }

    void connectionAdded (Connection& connection) override
    {
        auto newComponent = std::make_unique<ConnectionComponent> (connection);
        addAndMakeVisible (newComponent.get());
        components.push_back (std::move (newComponent));

        updateSize();
    }

    void updateSize()
    {
        int height = 0;
        for (auto& c : components)
            height += spacing + c->getHeight();

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
