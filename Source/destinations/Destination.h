
#pragma once

#include <JuceHeader.h>
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
private:
    TrackerInterface& tracker;
};
