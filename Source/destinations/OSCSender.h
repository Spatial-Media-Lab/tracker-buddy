
#pragma once

#include <iostream>
#include <string>

#include <JuceHeader.h>

#include "Destination.h"
#include "../tracker/RotationData.h"


class OSCSender : public Destination
{
    struct MessageTemplate
    {
        MessageTemplate (juce::String oscAddress) : address (oscAddress)
        {}

        juce::OSCAddressPattern address;
        std::vector<float RotationData::*> arguments;
    };

    class Component : public Destination::Widget
    {
        
    public:
        Component (OSCSender& o) : Widget (o), oscSender (o)
        {
            editor.setMultiLine (true);
            editor.setReturnKeyStartsNewLine (true);
            editor.onFocusLost = [&] () { oscSender.setProtocol (editor.getText()); };
            addAndMakeVisible (editor);
        }

        ~Component() override = default;

        void paint (juce::Graphics& g) override
        {
            using namespace juce;

            auto b = getLocalBounds();
            g.setColour (Colours::limegreen);
            g.fillRoundedRectangle (b.toFloat(), 0.2 * b.getHeight());

            g.setColour (Colours::white);

            g.drawFittedText ("OSCSender", b, Justification::topLeft, 1);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (10);
            editor.setBounds (bounds);
        }

        int getWidgetHeight() override
        {
            return 110;
        }



    private:
        OSCSender& oscSender;

        juce::TextEditor editor;
    };

public:
    static constexpr char name[] = "OSCSender";

    OSCSender (TrackerInterface& source) : Destination (source)
    {
    }

    ~OSCSender() override {};

    void newQuaternionData (const Quaternion& q) override
    {
        auto data = RotationData (q);

        for (auto& p : protocol)
        {
            auto m = juce::OSCMessage (p->address);
            std::cout << p->address.toString();

            for (auto& a : p->arguments)
            {
                std::cout << " " << std::to_string (data.*a);
                m.addFloat32 (data.*a);
            }

            std::cout << std::endl;
        }
    }


    std::unique_ptr<Widget> createWidget() override
    {
        return std::make_unique<OSCSender::Component> (*this);
    }

    std::string getName() const override
    {
        return name;
    }

    juce::Result setProtocol (juce::String protocolToSet)
    {
        protocol.clear();

        auto lines = juce::StringArray::fromLines (protocolToSet);

        for (auto& line : lines)
        {
            line.trim();
            if (! line.isEmpty())
            {
                auto args = juce::StringArray::fromTokens (line, true);

                std::unique_ptr<MessageTemplate> p;
                try
                {
                    p = std::make_unique<MessageTemplate> (args[0]);
                }
                catch (juce::OSCFormatError error)
                {
                    return juce::Result::fail (error.description);
                }


                for (int i = 1; i < args.size(); ++i)
                {
                    if (auto ptr = RotationData::getMemberPointer (args[i]); ptr)
                        p->arguments.push_back (ptr);
                    else
                        return juce::Result::fail ("'" + args[i] + "' is not a valid argument!");
                }

                protocol.push_back (std::move (p));
            }
        }

        return juce::Result::ok();
    }



private:
    std::vector<std::unique_ptr<MessageTemplate>> protocol;
    juce::OSCSender oscSender;
};
