

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
        bounds.reduce (3, 3);

        g.setColour (connected ? juce::Colours::limegreen : juce::Colours::red);
        g.drawEllipse (bounds, 2.0f);

        if (active)
        {
            bounds.reduce (3, 3);
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
    class ColourPicker : public juce::Component, private juce::ChangeListener
    {
    public:
        ColourPicker (TrackerInterface& tracker) : t (tracker) {}
        ~ColourPicker() = default;

        void paint (juce::Graphics& g) override
        {
            g.setColour (currentColour);
            g.fillRoundedRectangle (getLocalBounds().toFloat(), 4.0f);
        }

        void mouseUp (const juce::MouseEvent& event) override
        {
            auto colourSelector = std::make_unique<juce::ColourSelector> (juce::ColourSelector::showColourspace);
            colourSelector->setName ("LED Color");
            colourSelector->setCurrentColour (currentColour);
            colourSelector->addChangeListener (this);
            colourSelector->setColour (juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
            colourSelector->setSize (250, 250);

            juce::CallOutBox::launchAsynchronously (std::move (colourSelector), getScreenBounds(), nullptr);
        }

        void changeListenerCallback (juce::ChangeBroadcaster* source) override
        {
            if (juce::ColourSelector* cs = dynamic_cast<juce::ColourSelector*> (source))
            {
                currentColour = cs->getCurrentColour();

                t.setHue (currentColour.getHue() * 360);
                t.setSaturation (currentColour.getSaturation());
                t.setBrightness (currentColour.getBrightness());
                repaint();
            }
        }

        void setHSV (float hue, float saturation, float brightness)
        {
            currentColour = juce::Colour::fromHSV (hue, saturation, brightness, 1.0f);
            repaint();
        }

        juce::Colour getColour() const
        {
            return currentColour;
        }

    private:
        TrackerInterface& t;
        juce::Colour currentColour;
    };

public:
    static int getIdealHeight() { return 80; };

    TrackerInterfaceComponent (TrackerInterface& interface) :
    trackerInterface (interface),
    colourPicker (interface)
    {
        activity.setConnected (trackerInterface.isDeviceOpen());
        addAndMakeVisible (activity);

        addAndMakeVisible (colourPicker);

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
        // LED COLOR
        auto currentColour = colourPicker.getColour();
        auto hue = config.getProperty ("hue", static_cast<float> (currentColour.getHue()));
        auto saturation = config.getProperty ("saturation", static_cast<float> (currentColour.getSaturation()));
        auto brightness = config.getProperty ("brightness", static_cast<float> (currentColour.getBrightness()));

        colourPicker.setHSV (float (hue) / 360.0f, saturation, brightness);
        //

        name = config.getProperty ("name", "");
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::cornflowerblue.withAlpha (0.1f));
        g.fillAll();

        g.setColour (juce::Colours::white);

        auto row = getLocalBounds().removeFromTop (25);
        row.removeFromLeft (25 + 4);
        g.setFont (20.0f);
        g.drawText (name, row, juce::Justification::left);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto row = bounds.removeFromTop (25);

        activity.setBounds (row.removeFromLeft (25));
        row.removeFromLeft (10);

        bounds.removeFromTop (3);

        row = bounds.removeFromTop (25);
        colourPicker.setBounds (row.removeFromLeft (40));
        row.removeFromLeft (50);
        writeConfigButton.setBounds (row);
    }

    void timerCallback() override
    {
        activity.setActive (active);
        active = false;
    }


private:
    TrackerInterface& trackerInterface;

    juce::String name;

    bool active = false;
    ActivityComponent activity;
    ColourPicker colourPicker;
    juce::TextButton writeConfigButton;
};
