
#pragma once

#include <JuceHeader.h>


class RemoveButton : public juce::Component
{
public:
    RemoveButton()
    {
        mouseOver = false;
        confirming = false;
    }

    ~RemoveButton() = default;

    void paint (juce::Graphics& g) override
    {
        juce::Colour background = juce::Colour (54, 70, 84);
        juce::Colour colour     = juce::Colours::orangered;

        auto bounds = getLocalBounds().reduced (2);

        if (bounds.getWidth() > bounds.getHeight())
            bounds.setWidth (bounds.getHeight());
        else
            bounds.setHeight (bounds.getWidth());

        auto floatBounds = bounds.toFloat();
        auto centre = floatBounds.getCentre();

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

        juce::Path line;
        line.startNewSubPath (centre.getX() - 0.325f * w, centre.getY());

        if (confirming)
        {
            line.lineTo (centre.getX() - 0.125f * w, centre.getY() + 0.225f * w);
            line.lineTo (centre.getX() + 0.225f * w, centre.getY() - 0.225f * w);
        }
        else
        {
            line.lineTo (centre.getX() + 0.325f * w, centre.getY());
        }

        g.strokePath (line, juce::PathStrokeType (2.0f));
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
        if (! confirming)
        {
            confirming = true;
            juce::Timer::callAfterDelay (1000, [this]() { confirming = false; repaint(); });
            repaint();
        }
        else
        {
            if (onClick)
            {
                mouseOver = false;
                repaint();
                onClick();
            }
        }
    }

    std::function<void()> onClick;

private:
    bool confirming;
    bool mouseOver;
};
