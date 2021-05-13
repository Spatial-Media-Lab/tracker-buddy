
#pragma once

#include <JuceHeader.h>


class AddButton : public juce::Component
{
public:
    AddButton()
    {
        mouseOver = false;
    }

    ~AddButton() = default;

    void paint (juce::Graphics& g) override
    {
        juce::Colour background = juce::Colour (54, 70, 84);
        juce::Colour colour     = juce::Colours::limegreen;

        auto bounds = getLocalBounds().reduced (2);

        if (bounds.getWidth() > bounds.getHeight())
            bounds.setWidth (bounds.getHeight());
        else
            bounds.setHeight (bounds.getWidth());

        auto floatBounds = bounds.toFloat();


        if (mouseOver)
        {
            g.setColour (colour);
            g.fillEllipse (floatBounds);
            g.setColour (background);
        }
        else
        {
            g.setColour (background);
            g.fillEllipse (floatBounds);
            g.setColour (colour);
            g.drawEllipse (floatBounds, 1.5f);
        }


        const float w = floatBounds.getWidth();
        auto line = juce::Rectangle<float> (0, 0, 0.65f * w, 0.12f * w);
        line.setCentre (floatBounds.getCentre());
        g.fillRect (line);

        line = juce::Rectangle<float> (0, 0, 0.12f * w, 0.65f * w);
        line.setCentre (floatBounds.getCentre());
        g.fillRect (line);

    }

    void mouseEnter (const juce::MouseEvent& e) override
    {
        mouseOver = true;
        repaint();
    }

    void mouseExit (const juce::MouseEvent& e) override
    {
        mouseOver = false;
        repaint();
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (onClick)
        {
            mouseOver = false;
            repaint();
            onClick();
        }
    }

    std::function<void()> onClick;

private:
    bool mouseOver;
};
