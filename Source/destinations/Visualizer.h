
#pragma once

#include <iostream>
#include <JuceHeader.h>

#include "Destination.h"
#include "../tracker/RotationData.h"


class Visualizer : public Destination
{
    class Component : public Destination::Widget, private juce::Timer
    {
    public:
        Component (Visualizer& v) : Widget (v), visualizer (v)
        {
            qw.setRange (-1.0, 1.0, 0.0);
            qx.setRange (-1.0, 1.0, 0.0);
            qy.setRange (-1.0, 1.0, 0.0);
            qz.setRange (-1.0, 1.0, 0.0);

            qw.setNumDecimalPlacesToDisplay (3);
            qx.setNumDecimalPlacesToDisplay (3);
            qy.setNumDecimalPlacesToDisplay (3);
            qz.setNumDecimalPlacesToDisplay (3);

            addAndMakeVisible (qw);
            addAndMakeVisible (qx);
            addAndMakeVisible (qy);
            addAndMakeVisible (qz);

            
            yaw.setRange   (-180.0, 180.0, 0.0);
            pitch.setRange (-180.0, 180.0, 0.0);
            roll.setRange  (-180.0, 180.0, 0.0);

            yaw.setNumDecimalPlacesToDisplay (1);
            pitch.setNumDecimalPlacesToDisplay (1);
            roll.setNumDecimalPlacesToDisplay (1);

            addAndMakeVisible (yaw);
            addAndMakeVisible (pitch);
            addAndMakeVisible (roll);

            startTimer (50);
        }

        void paint (juce::Graphics& g) override
        {
            using namespace juce;

            auto b = getLocalBounds();
            g.setColour (Colours::cornflowerblue);
            g.fillRoundedRectangle (b.toFloat(), 0.2 * b.getHeight());
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (5);

            const auto half = (bounds.getWidth() - 8) / 2;

            auto leftCol = bounds.removeFromLeft (half);
            auto rightCol = bounds.removeFromLeft (half);

            qw.setBounds (leftCol.removeFromTop (10));
            qx.setBounds (leftCol.removeFromTop (10));
            qy.setBounds (leftCol.removeFromTop (10));
            qz.setBounds (leftCol.removeFromTop (10));

            yaw.setBounds (rightCol.removeFromTop (10));
            pitch.setBounds (rightCol.removeFromTop (10));
            roll.setBounds (rightCol.removeFromTop (10));
        }

        int getWidgetHeight() override
        {
            return 100;
        }

        void timerCallback() override
        {
            auto data = RotationData (visualizer.getQuaternion());

            qw.setValue (data.qw);
            qx.setValue (data.qx);
            qy.setValue (data.qy);
            qz.setValue (data.qz);

            yaw.setValue (data.yawDegrees);
            pitch.setValue (data.pitchDegrees);
            roll.setValue (data.rollDegrees);
        }

    private:
        Visualizer& visualizer;

        juce::Slider qw;
        juce::Slider qx;
        juce::Slider qy;
        juce::Slider qz;

        juce::Slider yaw;
        juce::Slider pitch;
        juce::Slider roll;
    };

public:
    static constexpr char name[] = "Visualizer";

    Visualizer (TrackerInterface& source) : Destination (source)
    {
    }

    ~Visualizer() override {};
    
    void newQuaternionData (const Quaternion& q) override
    {
        quat = q;
    }


    std::unique_ptr<Widget> createWidget() override
    {
        return std::make_unique<Visualizer::Component> (*this);
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
