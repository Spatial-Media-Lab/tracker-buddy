
#pragma once


#include <array>
#include <memory.h>
#include "../tracker/TrackerInterface.h"

class Destination : public TrackerInterface::Listener
{
public:
    Destination (TrackerInterface& source) : tracker (source)
    {
        tracker.addListener (this);
    }

    virtual ~Destination()
    {
        tracker.removeListener (this);
    }

    virtual std::unique_ptr<juce::Component> createComponent() = 0;
    virtual std::string getName() const = 0;
private:
    TrackerInterface& tracker;
};




#include "Visualizer.h"



struct DestinationFactory
{
    static std::array<std::string, 1> getListOfDestinations()
    {
        return { "Visualizer" };
    };

    static std::unique_ptr<Destination> createDestination (TrackerInterface& tracker, std::string destinationName)
    {
        if (destinationName == "Visualizer")
            return std::make_unique<Visualizer> (tracker);

        return nullptr;
    }
};
