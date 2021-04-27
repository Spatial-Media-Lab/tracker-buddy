

#pragma once

#include <JuceHeader.h>

#include "TrackerInterface.h"
#include "Quaternion.h"


class TrackerInterfaceComponent : public juce::Component, public TrackerInterface::Listener
{
public:
    TrackerInterfaceComponent (TrackerInterface& interface) :
    trackerInterface (interface)
    {
        trackerInterface.addListener (this);

        hueSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        hueSlider.setRange (0.0, 360.0);
        hueSlider.onValueChange = [&] () { trackerInterface.setHue (hueSlider.getValue()); };
        addAndMakeVisible (hueSlider);
    }

    ~TrackerInterfaceComponent() override
    {
        trackerInterface.removeListener (this);
    }


    void midiInputOpened() override {}
    void midiInputClosed() override {}
    void midiOpenError() override {}
    void newQuaternionData (const Quaternion& newQuaternion) override { }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        hueSlider.setBounds (getLocalBounds());
    }


private:
    TrackerInterface& trackerInterface;

    juce::Slider hueSlider;
};
