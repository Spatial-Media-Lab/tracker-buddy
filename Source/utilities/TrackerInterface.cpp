

#include "TrackerInterface.h"

TrackerInterface::TrackerInterface (MidiDeviceManager& manager, InputOutputPair device) :
inputOutputPair (device),
midiDeviceManager (manager)
{
    connected = false;

    startTimer (1000);

    midiDeviceManager.addListener (this);
}

TrackerInterface::~TrackerInterface()
{
    midiDeviceManager.removeListener (this);
    closeMidiDevices();
}


void TrackerInterface::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage &message)
{
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
}

void TrackerInterface::openMidiDevices()
{
    const std::unique_lock<std::mutex> scopedLock (changingMidiDevice);

    midiInput = juce::MidiInput::openDevice (inputOutputPair.inputDevice.identifier, this);
    midiOutput = juce::MidiOutput::openDevice (inputOutputPair.outputDevice.identifier);

    if (midiInput == nullptr || midiOutput == nullptr)
    {
        midiInput = nullptr;
        midiOutput = nullptr;

        listeners.call ([] (Listener& l) { l.midiOpenError(); });
        return;
    }

    midiInput->start();

    listeners.call ([] (Listener& l) { l.midiInputOpened(); });
}

void TrackerInterface::closeMidiDevices()
{
    const std::unique_lock<std::mutex> lock (changingMidiDevice);

    if (midiInput != nullptr)
    {
        midiInput->stop();
        midiInput.reset();
        listeners.call ([] (Listener& l) { l.midiInputClosed(); });
    }

    return;
}

// ============================================================================================
