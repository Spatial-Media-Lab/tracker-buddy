
#pragma once

#include <iostream>
#include <string>

#include <JuceHeader.h>

#include "Destination.h"
#include "../tracker/RotationData.h"
#include "OSCSenderPlus.h"

class OSCSender : public Destination, public OSCSenderPlus
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
            expanded = false;

            editor.setMultiLine (true);
            editor.setReturnKeyStartsNewLine (true);
            editor.onFocusLost = [&] () { oscSender.setProtocol (editor.getText()); };
            addAndMakeVisible (editor);

            addAndMakeVisible (ip);
            addAndMakeVisible (port);

            connectButton.setButtonText ("connect");
            connectButton.onClick = [&] () { toggleConnection(); };
            addAndMakeVisible (connectButton);
        }

        ~Component() override = default;

        void toggleConnection()
        {
            if (oscSender.isConnected())
                oscSender.disconnect();
            else
                oscSender.connect (ip.getText(), port.getText().getIntValue());

            connectButton.setButtonText (oscSender.isConnected() ? "disconnect" : "connect");
        }


        void paint (juce::Graphics& g) override
        {
            using namespace juce;

            auto b = getLocalBounds();
            g.setColour (Colours::limegreen.withAlpha (0.1f));
            g.fillRoundedRectangle (b.toFloat(), 0.2 * b.getHeight());

            g.setColour (Colours::white);

            g.drawFittedText ("OSCSender", b, Justification::topLeft, 1);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (12);
            auto row = bounds.removeFromTop (20);
            connectButton.setBounds (row.removeFromRight (50));
            row.removeFromRight (5);
            port.setBounds (row.removeFromRight (50));
            row.removeFromRight (5);
            ip.setBounds (row);

            bounds.removeFromTop (10);
            editor.setBounds (bounds);
        }

        int getWidgetHeight() override
        {
            return expanded ? 250 : 150;
        }

        void mouseDown (const juce::MouseEvent& event) override
        {
            expanded = ! expanded;
            setSize (getWidth(), getWidgetHeight());
        }


    private:
        OSCSender& oscSender;

        bool expanded;

        juce::TextEditor ip;
        juce::TextEditor port;
        juce::TextButton connectButton;

        juce::TextEditor editor;
    };

public:
    static constexpr char name[] = "OSCSender";

    OSCSender (TrackerInterface& source) : Destination (source), protocolValid (juce::Result::ok())
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

            if (isConnected())
                send (m);
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
        protocolString = protocolToSet;
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
                    protocolValid = juce::Result::fail (error.description);
                    return protocolValid;
                }


                for (int i = 1; i < args.size(); ++i)
                {
                    if (auto ptr = RotationData::getMemberPointer (args[i]); ptr)
                        p->arguments.push_back (ptr);
                    else
                    {
                        protocolValid = juce::Result::fail ("'" + args[i] + "' is not a valid argument!");
                        return protocolValid;
                    }
                }

                protocol.push_back (std::move (p));
            }
        }

        protocolValid = juce::Result::ok();
        return protocolValid;
    }



private:
    juce::String protocolString;
    juce::Result protocolValid;
    std::vector<std::unique_ptr<MessageTemplate>> protocol;
};
