
#pragma once
#include <JuceHeader.h>


class ConnectionManager
{
public:
    struct Listener
    {
        virtual void connectionAdded (Connection& connection) {}
        virtual void connectionRemoved (Connection& connection) {}
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

    void removeConnection (Connection* connectionToRemove)
    {
        if (! connectionToRemove)
            return;

        for (auto& c : connections)
        {
            if (c.get() == connectionToRemove)
            {
                listeners.call ([&] (Listener& l) { l.connectionRemoved (*connectionToRemove); });

                connections.remove (c);
                return;
            }
        }
    }

    const std::list<std::unique_ptr<Connection>>& getConnections() const { return connections; };

private:
    juce::ListenerList<Listener> listeners;
    std::list<std::unique_ptr<Connection>> connections;
};
