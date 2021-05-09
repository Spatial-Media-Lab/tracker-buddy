
#pragma once
#include "Quaternion.h"
#include <JuceHeader.h>

#define _USE_MATH_DEFINES
#include <math.h>

struct RotationData
{
    RotationData (const Quaternion& q) :
    qw (q.w),
    qx (q.x),
    qy (q.y),
    qz (q.z)
    {
        auto ypr = q.toTaitBryanAngles();
        yawRadians = ypr.yaw;
        pitchRadians = ypr.pitch;
        rollRadians = ypr.roll;

        yawDegrees = rad2deg (yawRadians);
        pitchDegrees = rad2deg (pitchRadians);
        rollDegrees = rad2deg (rollRadians);
    }


    static float deg2rad (float deg)
    {
        return M_PI * deg / 180;
    }

    static float rad2deg (float deg)
    {
        return 180.0f * deg / M_PI;
    }

    static float RotationData::* getMemberPointer (const juce::String& memberName)
    {
        if (memberName == "qw")
            return &RotationData::qw;
        else if (memberName == "qx")
            return &RotationData::qx;
        else if (memberName == "qy")
            return &RotationData::qy;
        else if (memberName == "qz")
            return &RotationData::qz;

        else if (memberName == "yawDegrees")
            return &RotationData::yawDegrees;
        else if (memberName == "pitchDegrees")
            return &RotationData::pitchDegrees;
        else if (memberName == "rollDegrees")
            return &RotationData::rollDegrees;

        else if (memberName == "yawRadians")
            return &RotationData::yawRadians;
        else if (memberName == "pitchRadians")
            return &RotationData::pitchRadians;
        else if (memberName == "rollRadians")
            return &RotationData::rollRadians;

        else
            return nullptr;
    }

    float qw;
    float qx;
    float qy;
    float qz;

    float yawDegrees;
    float pitchDegrees;
    float rollDegrees;

    float yawRadians;
    float pitchRadians;
    float rollRadians;

    RotationData() = delete;
};
