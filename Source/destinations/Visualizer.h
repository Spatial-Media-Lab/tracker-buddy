
#pragma once

#include <iostream>
#include <JuceHeader.h>

#include "Destination.h"


class Visualizer : public Destination
{
    class Component : public Destination::Widget, private juce::Timer
    {
        struct Vec
        {
            float x, y, z;
            juce::String label;
            juce::Colour colour;
        };

    public:
        Component (Visualizer& v) : Widget (v), visualizer (v)
        {
            startTimer (50);
        }

        void paint (juce::Graphics& g) override
        {
            using namespace juce;

            auto b = getLocalBounds();
            g.fillAll (Colours::cornflowerblue.withAlpha(0.1f));

            auto centre = b.toFloat().getCentre();
            auto radius = 0.25f * b.getHeight();

            float fx = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
            float fy = 2 * q.x * q.y + 2* q.w * q.z;
            float fz = 2 * q.x * q.z - 2* q.w * q.y;

            float lx = 2 * q.x * q.y - 2 * q.w * q.z;
            float ly = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
            float lz = 2 * q.y * q.z + 2 * q.w * q.x;

            float rx = -lx;
            float ry = -ly;
            float rz = -lz;

            std::vector<Vec> elements (3);
            elements.push_back ({lx, ly, lz, "L", Colours::cornflowerblue});
            elements.push_back ({fx, fy, fz, "F", Colours::white});
            elements.push_back ({rx, ry, rz, "R", Colours::red});

            if (lz < 0)
                for (auto i = elements.begin(); i != elements.end(); ++i)
                    drawThumb (g, centre, *i, radius);
            else
                for (auto i = elements.rbegin(); i != elements.rend(); ++i)
                    drawThumb (g, centre, *i, radius);
        }

        void resized() override
        {
        }

        int getWidgetHeight() override
        {
            return 150;
        }

        void timerCallback() override
        {
            auto data = visualizer.getQuaternion();
            if (q != data)
            {
                q = data;
                repaint();
            }
        }

    private:
        void drawThumb (juce::Graphics& g, juce::Point<float> centre, Vec v, float r)
        {
            auto cr = 8 + 4 * v.z;
            auto circ = juce::Rectangle<float> (-v.y * r + centre.getX() - cr,
                                            -v.x * r + centre.getY() - cr,
                                          2 * cr, 2 * cr);

            g.setColour (v.colour);
            g.fillRoundedRectangle (circ.toFloat(), circ.getHeight());

            g.setColour (juce::Colours::black);
            g.setFont (2 * cr);
            g.drawText (v.label, circ, juce::Justification::centred);
        }

        Visualizer& visualizer;
        Quaternion q;
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
