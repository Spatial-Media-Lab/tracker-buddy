

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
    static int getIdealHeight() { return 80; };

    TrackerInterfaceComponent (TrackerInterface& interface) :
    trackerInterface (interface)
    {
        activity.setConnected (trackerInterface.isDeviceOpen());
        addAndMakeVisible (activity);

        hueSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        hueSlider.setRange (0.0, 360.0);
        hueSlider.onValueChange = [&] () { trackerInterface.setHue (hueSlider.getValue()); };
        addAndMakeVisible (hueSlider);

        satSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        satSlider.setRange (0.0, 1.0);
        satSlider.onValueChange = [&] () { trackerInterface.setSaturation (satSlider.getValue()); };
        addAndMakeVisible (satSlider);

        brightSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        brightSlider.setRange (0.0, 1.0);
        brightSlider.onValueChange = [&] () { trackerInterface.setBrightness (brightSlider.getValue()); };
        addAndMakeVisible (brightSlider);


        writeConfigButton.setButtonText ("write");
        writeConfigButton.onClick = [&] () { trackerInterface.writeConfig(); };
        addAndMakeVisible (writeConfigButton);

        trackerInterface.addListener (this);
        trackerInterface.requestConfig();
        
        startTimer (30);
    }

    ~TrackerInterfaceComponent() override
    {
        trackerInterface.removeListener (this);
    }


    void connected() override
    {
        activity.setConnected (true);
        trackerInterface.requestConfig();
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

    void configurationReceived (juce::var config) override
    {
        auto hue = config.getProperty ("hue", static_cast<float> (hueSlider.getValue()));
        auto saturation = config.getProperty ("saturation", static_cast<float> (satSlider.getValue()));
        auto brightness = config.getProperty ("brightness", static_cast<float> (brightSlider.getValue()));

        hueSlider.setValue (hue, juce::dontSendNotification);
        satSlider.setValue (saturation, juce::dontSendNotification);
        brightSlider.setValue (brightness, juce::dontSendNotification);
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::cornflowerblue.withAlpha (0.1f));
        g.fillAll();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        activity.setBounds (bounds.removeFromLeft (bounds.getHeight()));
        bounds.removeFromLeft (10);

        auto width = bounds.getWidth() / 4;
        hueSlider.setBounds (bounds.removeFromLeft (width));
        satSlider.setBounds (bounds.removeFromLeft (width));
        brightSlider.setBounds (bounds.removeFromLeft (width));
        writeConfigButton.setBounds (bounds.removeFromLeft (width));
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
    juce::Slider satSlider;
    juce::Slider brightSlider;
    juce::TextButton writeConfigButton;
};
