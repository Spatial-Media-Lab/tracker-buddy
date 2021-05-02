
#pragma once

#include <iostream>
#include <JuceHeader.h>

#include "Destination.h"



class Visualizer : public Destination
{
    class Component : public juce::Component, private juce::Timer
    {
    public:
        Component (Visualizer& v) : visualizer (v)
        {
            startTimer (50);
        }

        void paint (juce::Graphics& g) override
        {
            using namespace juce;

            auto b = getLocalBounds();
            g.setColour (Colours::white);

            auto q = visualizer.getQuaternion();

            auto text = String (q.w, 3) + " " +
                        String (q.x, 3) + " " +
                        String (q.y, 3) + " " +
                        String (q.z, 3);
            g.drawFittedText (text, b, Justification::centred, 1);
        }

        void timerCallback() override
        {
            repaint();
        }

    private:
        Visualizer& visualizer;
    };

public:
    Visualizer (TrackerInterface& source) : Destination (source)
    {

    }

    ~Visualizer() override {};
    
    void newQuaternionData (const Quaternion& q) override
    {
        quat = q;
    }


    std::unique_ptr<juce::Component> createComponent() override
    {
        return std::make_unique<Visualizer::Component> (*this);
    }

    std::string getName() const override
    {
        return "Visualizer";
    }

    Quaternion getQuaternion()
    {
        return quat;
    }

private:
    Quaternion quat;
};
