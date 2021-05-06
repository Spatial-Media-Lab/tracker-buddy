
#pragma once


#include <array>
#include <memory.h>
#include "../tracker/TrackerInterface.h"

class Destination : public TrackerInterface::Listener
{
public:
    struct Widget : public juce::Component
    {
        virtual int getWidgetHeight() = 0;
        ~Widget() override = default;
    };
    
    Destination (TrackerInterface& source) : tracker (source)
    {
        tracker.addListener (this);
    }

    virtual ~Destination()
    {
        tracker.removeListener (this);
    }

    virtual std::unique_ptr<Widget> createWidget() = 0;
    virtual std::string getName() const = 0;
private:
    TrackerInterface& tracker;
};



#include "Visualizer.h"
#include "OSCSender.h"


struct DestinationFactory
{
    static std::array<std::string, 2> getListOfDestinations()
    {
        return { Visualizer::name, OSCSender::name};
    };

    static std::unique_ptr<Destination> createDestination (TrackerInterface& tracker, std::string destinationName)
    {
        if (destinationName == Visualizer::name)
            return std::make_unique<Visualizer> (tracker);
        else if (destinationName == OSCSender::name)
            return std::make_unique<OSCSender> (tracker);

        return nullptr;
    }
};
