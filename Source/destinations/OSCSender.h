
#pragma once

#include <iostream>
#include <string>

#include <JuceHeader.h>

#include "Destination.h"



class OSCSender : public Destination
{
    class Component : public Destination::Widget
    {
        
    public:
        Component (OSCSender& v) : visualizer (v)
        {
        }

        void paint (juce::Graphics& g) override
        {
            using namespace juce;

            auto b = getLocalBounds();
            g.setColour (Colours::limegreen);
            g.fillRoundedRectangle (b.toFloat(), 0.2 * b.getHeight());

            g.setColour (Colours::white);

            g.drawFittedText ("OSCSender", b, Justification::topLeft, 1);
        }

        int getWidgetHeight() override
        {
            return 70;
        }


    private:
        OSCSender& visualizer;
    };

public:
    static constexpr char name[] = "OSCSender";

    OSCSender (TrackerInterface& source) : Destination (source)
    {

    }

    ~OSCSender() override {};

    void newQuaternionData (const Quaternion& q) override
    {
        quat = q;
    }


    std::unique_ptr<Widget> createWidget() override
    {
        return std::make_unique<OSCSender::Component> (*this);
    }

    std::string getName() const override
    {
        return name;
    }

    Quaternion getQuaternion()
    {
        return quat;
    }

private:
    Quaternion quat;
};
