#pragma once

#include <cmath>


struct TaitBryan
{
    float yaw;
    float pitch;
    float roll;
};

class Quaternion
{
public:
    float w{1};
    float x{0};
    float y{0};
    float z{0};

    constexpr Quaternion() = default;
    constexpr Quaternion (float qw, float qx, float qy, float qz) : w (qw), x (qx), y (qy), z (qz) {}

    float getLength() const
    {
        return std::sqrt (w * w + x * x + y * y + z * z);
    }

    void normalize()
    {
        const auto l = getLength();
        if (std::abs (l) > 0.000001f)
            scale(1.f / l);
    }

    void conjugate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    Quaternion getConjugate() const
    {
        return Quaternion (w, -x, -y, -z);
    }

    Quaternion operator* (const Quaternion &q) const
    {
        return Quaternion (w * q.w - x * q.x - y * q.y - z * q.z,
                           w * q.x + x * q.w + y * q.z - z * q.y,
                           w * q.y - x * q.z + y * q.w + z * q.x,
                           w * q.z + x * q.y - y * q.x + z * q.w);
    }

    void scale (float factor)
    {
        w *= factor;
        x *= factor;
        y *= factor;
        z *= factor;
    }

    TaitBryan toTaitBryanAngles() const
    {
        const float ysqr = y * y;

        float t0 = 2.0f * (w * z + x * y);
        float t1 = 1.0f - 2.0f * (ysqr + z * z);
        const float yaw = std::atan2 (t0, t1);

        t0 = 2.0f * (w * y - z * x);
        t0 = t0 > 1.0f ? 1.0f : t0;
        t0 = t0 < -1.0f ? -1.0f : t0;
        const float pitch = std::asin (t0);

        t0 = 2.0f * (w * x + y * z);
        t1 = 1.0f - 2.0f * (x * x + ysqr);
        const float roll = std::atan2 (t0, t1);

        return {yaw, pitch, roll};
    }
};
