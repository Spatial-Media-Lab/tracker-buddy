

#pragma once

#include <mutex>
#include <JuceHeader.h>

#include "Quaternion.h"



class TrackerInterface : private juce::MidiInputCallback, private juce::Timer, private juce::Value::Listener
{
public:
    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}
        virtual void midiInputOpened() {}
        virtual void midiInputClosed() {}
        virtual void midiOpenError() {}
        virtual void newQuaternionData (const Quaternion& newQuaternion) { }
    };

    static constexpr float factor = 1.0f / 16384;

    TrackerInterface();
    ~TrackerInterface() override;

    void setMidiDevice (juce::String midiDeviceName) { currentMidiDeviceName = midiDeviceName; }

    void closeMidiInput();
    bool isDeviceOpen() { return midiInput != nullptr; }

    juce::String getCurrentMidiDeviceName();

    Quaternion getQuaternion() const;
    inline void normalizeQuaternions();

    void addListener (Listener* newListener) { listeners.add (newListener); }
    void removeListener (Listener* listener) { listeners.remove (listener); }

    void notifyListeners();

    juce::Value& getValue() { return currentMidiDeviceName; }
    void valueChanged (juce::Value& value) override;

    void setHue (float hue)
    {
        const auto value = static_cast<int> (hue / 360 * 127);
        if (midiOutput)
            midiOutput->sendMessageNow (juce::MidiMessage::controllerEvent (1, 29, value));
    }

    // = Flags ========================================================================
    std::atomic<bool> activity;
    std::atomic<bool> connected;

private:
    void timerCallback() override;
    void openMidiInput();
    void handleIncomingMidiMessage (juce::MidiInput *source, const juce::MidiMessage &message) override;
    void updateQuaternionsFromYawPitchRoll();

    std::mutex changingMidiDevice;

    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;
    juce::Value currentMidiDeviceName;


    juce::ListenerList<Listener> listeners;

    bool unsettingDevice = false;

    int yawLsb = 0, pitchLsb = 0, rollLsb = 0;
    int qwLsb = 0, qxLsb = 0, qyLsb = 0, qzLsb = 0;
    float yawInRad = 0.0f, pitchInRad = 0.0f, rollInRad = 0.0f;
    juce::Atomic<float> qw = 1.0f, qx = 0.0f, qy = 0.0f, qz = 0.0f;

};

