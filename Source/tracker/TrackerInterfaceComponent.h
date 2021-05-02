

#pragma once

#include <JuceHeader.h>

#include "TrackerInterface.h"

class ActivityComponent : public juce::Component
{
public:
    ActivityComponent() = default;
    ~ActivityComponent() = default;

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.reduce (2, 2);

        g.setColour (connected ? juce::Colours::limegreen : juce::Colours::red);
        g.drawEllipse (bounds, 1.0f);

        if (active)
        {
            bounds.reduce (2, 2);
            g.fillEllipse (bounds);
        }
    }

    void setActive (bool shouldBeActive)
    {
        if (active == shouldBeActive)
            return;

        active = shouldBeActive;
        repaint();
    }

    void setConnected (bool shouldBeConnected)
    {
        if (connected == shouldBeConnected)
            return;

        connected = shouldBeConnected;
        repaint();
    }

private:
    bool active;
    bool connected;
};



class TrackerInterfaceComponent : public juce::Component, public TrackerInterface::Listener, public juce::Timer
{
public:
    TrackerInterfaceComponent (TrackerInterface& interface) :
    trackerInterface (interface)
    {
        activity.setConnected (trackerInterface.isDeviceOpen());
        addAndMakeVisible (activity);

        hueSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        hueSlider.setRange (0.0, 360.0);
        hueSlider.onValueChange = [&] () { trackerInterface.setHue (hueSlider.getValue()); };
        addAndMakeVisible (hueSlider);

        trackerInterface.addListener (this);
        startTimer (30);
    }

    ~TrackerInterfaceComponent() override
    {
        trackerInterface.removeListener (this);
    }


    void connected() override
    {
        activity.setConnected (true);
    }

    void disconnected() override
    {
        activity.setConnected (false);
    }

    void connectionFailed() override {}

    void newQuaternionData (const Quaternion& newQuaternion) override
    {
        active = true;
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        activity.setBounds (bounds.removeFromLeft (bounds.getHeight()));
        bounds.removeFromLeft (10);
        hueSlider.setBounds (bounds);
    }


    void timerCallback() override
    {
        activity.setActive (active);
        active = false;
    }



private:
    bool active = false;

    TrackerInterface& trackerInterface;

    ActivityComponent activity;
    juce::Slider hueSlider;
};
