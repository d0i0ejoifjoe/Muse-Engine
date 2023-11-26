#pragma once

#include "utils/public/utils.h"

#include <cmath>
#include <ostream>

namespace muse
{
class Vector3
{
  public:
    constexpr Vector3()
        : Vector3(0.0f)
    {
    }

    constexpr Vector3(float xyz)
        : Vector3(xyz, xyz, xyz)
    {
    }

    constexpr Vector3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    constexpr Vector3 &operator*=(const Vector3 &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;

        return *this;
    }

    constexpr Vector3 operator*(const Vector3 &v) const
    {
        return Vector3(*this) *= v;
    }

    constexpr Vector3 &operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;

        return *this;
    }

    constexpr Vector3 operator*(float scale) const
    {
        return Vector3(*this) *= scale;
    }

    constexpr Vector3 &operator/=(const Vector3 &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;

        return *this;
    }

    constexpr Vector3 operator/(const Vector3 &v) const
    {
        return Vector3(*this) /= v;
    }

    constexpr Vector3 &operator/=(float scale)
    {
        x /= scale;
        y /= scale;
        z /= scale;

        return *this;
    }

    constexpr Vector3 operator/(float scale) const
    {
        return Vector3(*this) /= scale;
    }

    constexpr Vector3 &operator+=(const Vector3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    constexpr Vector3 operator+(const Vector3 &v) const
    {
        return Vector3(*this) += v;
    }

    constexpr Vector3 &operator+=(float scale)
    {
        x += scale;
        y += scale;
        z += scale;

        return *this;
    }

    constexpr Vector3 operator+(float scale) const
    {
        return Vector3(*this) += scale;
    }

    constexpr Vector3 &operator-=(const Vector3 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    constexpr Vector3 operator-(const Vector3 &v) const
    {
        return Vector3(*this) -= v;
    }

    constexpr Vector3 &operator-=(float scale)
    {
        x -= scale;
        y -= scale;
        z -= scale;

        return *this;
    }

    constexpr Vector3 operator-(float scale) const
    {
        return Vector3(*this) -= scale;
    }

    constexpr Vector3 operator-() const
    {
        return Vector3{-x, -y, -z};
    }

    constexpr float magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    static constexpr float magnitude(const Vector3 &v)
    {
        return v.magnitude();
    }

    constexpr float dot(const Vector3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    static constexpr float dot(const Vector3 &a, const Vector3 &b)
    {
        return a.dot(b);
    }

    constexpr Vector3 cross(const Vector3 &other) const
    {
        return Vector3{y * other.z - z * other.y,
                       z * other.x - x * other.z,
                       x * other.y - y * other.x};
    }

    static constexpr Vector3 cross(const Vector3 &a, const Vector3 &b)
    {
        return a.cross(b);
    }

    constexpr Vector3 lerp(const Vector3 &other, float alpha) const
    {
        return *this * (1.0f - alpha) + other * alpha;
    }

    static constexpr Vector3 lerp(const Vector3 &a, const Vector3 &b, float alpha)
    {
        return a.lerp(b, alpha);
    }

    constexpr Vector3 normalize() const
    {
        const auto len = magnitude();

        if (len != 0.0f)
        {
            return *this / len;
        }

        return Vector3{*this};
    }

    static constexpr Vector3 normalize(const Vector3 &v)
    {
        return v.normalize();
    }

    static constexpr float distance(const Vector3 &a, const Vector3 &b)
    {
        return (b - a).magnitude();
    }

    bool operator==(const Vector3 &other) const
    {
        return compare(x, other.x) && compare(y, other.y) && compare(z, other.z);
    }

    bool operator!=(const Vector3 &other) const
    {
        return !(*this == other);
    }

    float x;

    float y;

    float z;
};

inline std::ostream &operator<<(std::ostream &os, const Vector3 &vec)
{
    os << "x: " << vec.x << '\n'
       << "y: " << vec.y << '\n'
       << "z: " << vec.z << '\n';

    return os;
}

}