

#include "TrackerInterface.h"

TrackerInterface::TrackerInterface()
{
    activity = false;
    connected = false;
    currentMidiDeviceName.addListener (this);
    startTimer (1000);
}

TrackerInterface::~TrackerInterface()
{
    closeMidiInput();
}


void TrackerInterface::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage &message)
{
    activity = true;

    switch (message.getControllerNumber())
    {
        case 48: qwLsb = message.getControllerValue(); break;
        case 49: qxLsb = message.getControllerValue(); break;
        case 50: qyLsb = message.getControllerValue(); break;
        case 51: qzLsb = message.getControllerValue(); break;

        case 16:
            qw = 2 * (128 * message.getControllerValue() + qwLsb) * factor - 1.0f;
            break;

        case 17:
            qx = 2 * (128 * message.getControllerValue() + qxLsb) * factor - 1.0f;
            break;

        case 18:
            qy = 2 * (128 * message.getControllerValue() + qyLsb) * factor - 1.0f;
            break;

        case 19:
            qz = 2 * (128 * message.getControllerValue() + qzLsb) * factor - 1.0f;
            normalizeQuaternions();
            notifyListeners();
            break;
    }
}



Quaternion TrackerInterface::getQuaternion() const
{
    Quaternion quat (qw.get(), qx.get(), qy.get(), qz.get());
    quat.normalize();
    return quat;
}

inline void TrackerInterface::normalizeQuaternions()
{
    const auto magInv = 1.0f / std::sqrt (qw.get() * qw.get() + qx.get() * qx.get() + qy.get() * qy.get() + qz.get() * qz.get());
    qw = qw.get() * magInv;
    qx = qx.get() * magInv;
    qy = qy.get() * magInv;
    qz = qz.get() * magInv;
}

void TrackerInterface::notifyListeners()
{
    const Quaternion quat (qw.get(), qx.get(), qy.get(), qz.get());
    listeners.call ([=] (Listener& l) { l.newQuaternionData (quat); } );
}

void TrackerInterface::timerCallback()
{
    if (midiInput == nullptr && ! getCurrentMidiDeviceName().isEmpty())
        openMidiInput();
}

void TrackerInterface::openMidiInput()
{
    if (unsettingDevice)
        return;

    const juce::String deviceName = getCurrentMidiDeviceName();
    if (deviceName.isEmpty())
        return closeMidiInput(); // <- not sure if that syntax is totally wrong or brilliant!

    
    const std::unique_lock<std::mutex> scopedLock (changingMidiDevice);

    juce::StringArray devices = juce::MidiInput::getDevices();
    auto outputDevices = juce::MidiOutput::getAvailableDevices();

    for (auto& dev : devices)
        DBG (dev);

    for (auto& dev : outputDevices)
        DBG (dev.identifier);

    const int index = devices.indexOf (deviceName);
    const auto outDev = outputDevices[index].identifier;
    if (index != -1)
    {
        midiInput = juce::MidiInput::openDevice (index, this);
        midiOutput = juce::MidiOutput::openDevice (outDev);

        if (midiInput == nullptr)
        {
            listeners.call ([] (Listener& l) { l.midiOpenError(); });
            return;
        }

        midiInput->start();
        listeners.call ([] (Listener& l) { l.midiInputOpened(); });
    }

}

void TrackerInterface::closeMidiInput()
{
    const std::unique_lock<std::mutex> lock (changingMidiDevice);

    if (midiInput != nullptr)
    {
        midiInput->stop();
        midiInput.reset();
        listeners.call ([] (Listener& l) { l.midiInputClosed(); });
    }

    juce::ScopedValueSetter<bool> unset (unsettingDevice, true);
    currentMidiDeviceName = juce::String(); // hoping there's not actually a MidiDevice without a name!
    return;
}

juce::String TrackerInterface::getCurrentMidiDeviceName()
{
    return currentMidiDeviceName.toString();
}

void TrackerInterface::valueChanged (juce::Value& value)
{
    openMidiInput();
}

// ============================================================================================
