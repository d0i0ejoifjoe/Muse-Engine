#pragma once

#include "vector4.h"

#include <ostream>

namespace muse
{
class Quaternion
{
  public:
    constexpr Quaternion()
        : w(1.0f)
        , x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {
    }

    constexpr Quaternion(float x, float y, float z, float w)
        : w(w)
        , x(x)
        , y(y)
        , z(z)
    {
    }

    constexpr Quaternion(const Vector3 &axis, float angle)
        : Quaternion()
    {
        const auto half_angle = angle / 2.0f;
        const auto sin_angle = std::sin(half_angle);

        w = std::cos(half_angle);
        x = sin_angle * axis.x;
        y = sin_angle * axis.y;
        z = sin_angle * axis.z;

        *this = normalize();
    }

    constexpr Quaternion(float yaw, float pitch, float roll)
        : Quaternion()
    {
        const auto cy = std::cos(yaw * 0.5f);
        const auto sy = std::sin(yaw * 0.5f);
        const auto cp = std::cos(pitch * 0.5f);
        const auto sp = std::sin(pitch * 0.5f);
        const auto cr = std::cos(roll * 0.5f);
        const auto sr = std::sin(roll * 0.5f);

        w = cr * cp * cy + sr * sp * sy;
        x = sr * cp * cy - cr * sp * sy;
        y = cr * sp * cy + sr * cp * sy;
        z = cr * cp * sy - sr * sp * cy;
    }

    constexpr Quaternion &operator*=(const Quaternion &q)
    {
        Quaternion copy(*this);

        w = copy.w * q.w - copy.x * q.x - copy.y * q.y - copy.z * q.z;
        x = copy.w * q.x + copy.x * q.w + copy.y * q.z - copy.z * q.y;
        y = copy.w * q.y + copy.y * q.w + copy.z * q.x - copy.x * q.z;
        z = copy.w * q.z + copy.z * q.w + copy.x * q.y - copy.y * q.x;

        return *this;
    }

    constexpr Quaternion operator*(const Quaternion &q) const
    {
        return Quaternion(*this) *= q;
    }

    constexpr Quaternion &operator*=(float scale)
    {
        w *= scale;
        x *= scale;
        y *= scale;
        z *= scale;

        return *this;
    }

    constexpr Quaternion operator*(float scale) const
    {
        return Quaternion(*this) *= scale;
    }

    constexpr Quaternion &operator/=(float scale)
    {
        w /= scale;
        x /= scale;
        y /= scale;
        z /= scale;

        return *this;
    }

    constexpr Quaternion operator/(float scale) const
    {
        return Quaternion(*this) /= scale;
    }

    constexpr Quaternion &operator+=(float scale)
    {
        w += scale;
        x += scale;
        y += scale;
        z += scale;

        return *this;
    }

    constexpr Quaternion operator+(float scale) const
    {
        return Quaternion(*this) += scale;
    }

    constexpr Quaternion &operator-=(float scale)
    {
        w -= scale;
        x -= scale;
        y -= scale;
        z -= scale;

        return *this;
    }

    constexpr Quaternion operator-(float scale) const
    {
        return Quaternion(*this) -= scale;
    }

    constexpr Quaternion operator-() const
    {
        return Quaternion{-x, -y, -z, -w};
    }

    constexpr Quaternion normalize() const
    {
        const auto len = magnitude();

        if (len != 0.0f)
        {
            return *this / len;
        }

        return Quaternion{*this};
    }

    static constexpr Quaternion normalize(const Quaternion &q)
    {
        return q.normalize();
    }

    constexpr float magnitude() const
    {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    static constexpr float magnitude(const Quaternion &q)
    {
        return q.magnitude();
    }

    constexpr float dot(const Quaternion &other) const
    {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }

    static constexpr float dot(const Quaternion &a, const Quaternion &b)
    {
        return a.dot(b);
    }

    constexpr Quaternion slerp(const Quaternion &q, float alpha) const
    {
        auto dot = this->dot(q);

        Quaternion end{q};
        if (dot < 0.0f)
        {
            dot = -dot;
            end = -end;
        }

        float coeff1 = 0.0f;
        float coeff2 = 0.0f;

        constexpr auto epsilon = std::numeric_limits<float>::epsilon();
        if (1.0f - dot > epsilon)
        {
            float omega = 0.0f;
            float sin = 0.0f;
            omega = std::acos(dot);
            sin = std::sin(omega);
            coeff1 = std::sin((1.0f - alpha) * omega) / sin;
            coeff2 = std::sin(alpha * omega) / sin;
        }

        return Quaternion{x * coeff1 + end.x * coeff2,
                          y * coeff1 + end.y * coeff2,
                          z * coeff1 + end.z * coeff2,
                          w * coeff1 + end.w * coeff2};
    }

    static constexpr Quaternion slerp(const Quaternion &a, const Quaternion &b, float alpha)
    {
        return a.slerp(b, alpha);
    }

    bool operator==(const Quaternion &q) const
    {
        return compare(x, q.x) && compare(y, q.y) && compare(z, q.z) && compare(w, q.w);
    }

    bool operator!=(const Quaternion &q) const
    {
        return !(*this == q);
    }

    float w;

    float x;

    float y;

    float z;
};

inline std::ostream &operator<<(std::ostream &os, const Quaternion &q)
{
    os << "w: " << q.w << '\n'
       << "x: " << q.x << '\n'
       << "y: " << q.y << '\n'
       << "z: " << q.z << '\n';

    return os;
}
}
