

#pragma once

#include <JuceHeader.h>
#include <atomic>

class OSCSenderPlus : public juce::OSCSender
{
public:
    OSCSenderPlus()
    {
        connected = false;
    }

    bool connect (const juce::String& targetHostName, int portNumber)
    {
        hostName = targetHostName;
        port = portNumber;

        if (portNumber == -1 || targetHostName.isEmpty())
        {
            disconnect();
            connected = false;
            return true;
        }

        if (juce::OSCSender::connect (targetHostName, port))
        {
            connected = true;
            return true;
        }
        else
            return false;
    }

    bool disconnect()
    {
        if (OSCSender::disconnect())
        {
            connected = false;
            return true;
        }
        else
            return false;
    }

    int getPortNumber() const
    {
        return port;
    }

    juce::String getHostName() const
    {
        return hostName;
    }

    bool isConnected() const
    {
        return connected.load();
    }


private:
    juce::String hostName;
    int port = -1;
    std::atomic<bool> connected;
};

