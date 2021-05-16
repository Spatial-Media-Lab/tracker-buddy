
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
            g.setColour (Colours::cornflowerblue);



            auto centre = b.toFloat().getCentre();

            auto r = 0.25f * b.getHeight();

            {
                Vec p {1 - 2 * q.y * q.y - 2 * q.z * q.z,
                2 * q.x * q.y + 2* q.w * q.z,
                2 * q.x * q.z - 2* q.w * q.y};

                auto cr = 8 + 4 * p.z;
                auto circ = Rectangle<float> (-p.y * r + centre.getY() - cr,
                                               - p.x * r + centre.getY() - cr,
                                              2 * cr, 2 * cr);

                g.setColour (Colours::white);
                g.fillRoundedRectangle (circ.toFloat(), circ.getHeight());
            }

            {
                Vec p {2 * q.x * q.y - 2 * q.w * q.z,
                1 - 2 * q.x * q.x - 2 * q.z * q.z,
                2 * q.y * q.z + 2 * q.w * q.x};

                auto cr = 8 + 4 * p.z;
                auto circ = Rectangle<float> (-p.y * r + centre.getY() - cr,
                                               - p.x * r + centre.getY() - cr,
                                              2 * cr, 2 * cr);

                g.setColour (Colours::cornflowerblue);
                g.fillRoundedRectangle (circ.toFloat(), circ.getHeight());
            }

            {
                Vec p {2 * q.x * q.y - 2 * q.w * q.z,
                1 - 2 * q.x * q.x - 2 * q.z * q.z,
                2 * q.y * q.z + 2 * q.w * q.x};

                auto cr = 8 - 4 * p.z;
                auto circ = Rectangle<float> (p.y * r + centre.getY() - cr,
                                                p.x * r + centre.getY() - cr,
                                              2 * cr, 2 * cr);

                g.setColour (Colours::red);
                g.fillRoundedRectangle (circ.toFloat(), circ.getHeight());
            }



        }

        void resized() override
        {
        }

        int getWidgetHeight() override
        {
            return 100;
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
