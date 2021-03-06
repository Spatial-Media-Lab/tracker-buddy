

#pragma once

#include <mutex>
#include <JuceHeader.h>

#include "Quaternion.h"
#include "MidiDeviceManager.h"


class TrackerInterface : private juce::MidiInputCallback, private juce::Timer, public MidiDeviceManager::Listener
{
public:
    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}
        virtual void connected() {}
        virtual void disconnected() {}
        virtual void connectionFailed() {}
        virtual void newQuaternionData (const Quaternion& newQuaternion) {}
        virtual void configurationReceived (juce::var config) {}
    };

    static constexpr float factor = 1.0f / 16384;

    TrackerInterface (MidiDeviceManager& manager, InputOutputPair device);
    ~TrackerInterface() override;

    void devicesChanged (const juce::Array<InputOutputPair>& devices) override
    {
        if (isDeviceOpen())
        {
            if (! devices.contains (inputOutputPair))
                closeMidiDevices();
        }
        else
        {
            if (devices.contains (inputOutputPair))
            openMidiDevices();
        }
    }

    void closeMidiDevices();
    bool isDeviceOpen() { return midiInput != nullptr && midiOutput != nullptr; }


    Quaternion getQuaternion() const;
    inline void normalizeQuaternions();

    void addListener (Listener* newListener) { listeners.add (newListener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }

    void notifyListeners();

    void setHue (float hue)
    {
        const auto value = static_cast<int> (hue / 360 * 127);
        if (midiOutput)
            midiOutput->sendMessageNow (juce::MidiMessage::controllerEvent (1, 29, value));
    }

    void setSaturation (float saturation)
    {
        const auto value = static_cast<int> (saturation * 127);
        if (midiOutput)
            midiOutput->sendMessageNow (juce::MidiMessage::controllerEvent (1, 30, value));
    }

    void setBrightness (float brightness)
    {
        const auto value = static_cast<int> (brightness * 127);
        if (midiOutput)
            midiOutput->sendMessageNow (juce::MidiMessage::controllerEvent (1, 31, value));
    }

    void writeConfig()
    {
        const juce::String json = "}{\"com.versioduo.device\": {\"method\" : \"writeConfiguration\", \"configuration\" : {}}}";

        auto message = juce::MidiMessage::createSysExMessage (json.toUTF8(), json.getNumBytesAsUTF8());

        if (midiOutput)
            midiOutput->sendMessageNow (message);
    }

    void requestConfig()
    {
        const juce::String json = "}{\"com.versioduo.device\": {\"method\" : \"getAll\"}}";

        auto message = juce::MidiMessage::createSysExMessage (json.toUTF8(), json.getNumBytesAsUTF8());

        if (midiOutput)
            midiOutput->sendMessageNow (message);
    }

    // = Flags ========================================================================

    void openMidiDevices();

private:
    void timerCallback() override;

    void handleIncomingMidiMessage (juce::MidiInput *source, const juce::MidiMessage &message) override;

    MidiDeviceManager& midiDeviceManager;
    InputOutputPair inputOutputPair;

    std::mutex changingMidiDevice;

    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;

    juce::ListenerList<Listener> listeners;

    int yawLsb = 0, pitchLsb = 0, rollLsb = 0;
    int qwLsb = 0, qxLsb = 0, qyLsb = 0, qzLsb = 0;

    juce::Atomic<float> qw = 1.0f, qx = 0.0f, qy = 0.0f, qz = 0.0f;
};

