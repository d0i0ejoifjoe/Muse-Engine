#pragma once

#include "vector3.h"

#include <cassert>

namespace muse
{
class Vector4
{
  public:
    constexpr Vector4()
        : Vector4(0.0f)
    {
    }

    constexpr Vector4(float xyzw)
        : Vector4(xyzw, xyzw, xyzw, xyzw)
    {
    }

    constexpr Vector4(const Vector3 &xyz, float w = 1.0f)
        : Vector4(xyz.x, xyz.y, xyz.z, w)
    {
    }

    constexpr Vector4(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr Vector4 &operator*=(const Vector4 &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;

        return *this;
    }

    constexpr Vector4 operator*(const Vector4 &v) const
    {
        return Vector4(*this) *= v;
    }

    constexpr Vector4 &operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;
        w *= scale;

        return *this;
    }

    constexpr Vector4 operator*(float scale) const
    {
        return Vector4(*this) *= scale;
    }

    constexpr Vector4 &operator/=(const Vector4 &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;

        return *this;
    }

    constexpr Vector4 operator/(const Vector4 &v) const
    {
        return Vector4(*this) /= v;
    }

    constexpr Vector4 &operator/=(float scale)
    {
        x /= scale;
        y /= scale;
        y /= scale;
        w /= scale;

        return *this;
    }

    constexpr Vector4 operator/(float scale) const
    {
        return Vector4(*this) /= scale;
    }

    constexpr Vector4 &operator+=(const Vector4 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;

        return *this;
    }

    constexpr Vector4 operator+(const Vector4 &v) const
    {
        return Vector4(*this) += v;
    }

    constexpr Vector4 &operator+=(float scale)
    {
        x += scale;
        y += scale;
        z += scale;
        w += scale;

        return *this;
    }

    constexpr Vector4 operator+(float scale) const
    {
        return Vector4(*this) += scale;
    }

    constexpr Vector4 &operator-=(const Vector4 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;

        return *this;
    }

    constexpr Vector4 operator-(const Vector4 &v) const
    {
        return Vector4(*this) -= v;
    }

    constexpr Vector4 &operator-=(float scale)
    {
        x -= scale;
        y -= scale;
        z -= scale;
        w -= scale;

        return *this;
    }

    constexpr Vector4 operator-(float scale) const
    {
        return Vector4(*this) -= scale;
    }

    constexpr Vector4 operator-() const
    {
        return Vector4{-x, -y, -z, -w};
    }

    constexpr float magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    static constexpr float magnitude(const Vector4 &v)
    {
        return v.magnitude();
    }

    constexpr float dot(const Vector4 &other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    static constexpr float dot(const Vector4 &a, const Vector4 &b)
    {
        return a.dot(b);
    }

    constexpr Vector4 lerp(const Vector4 &other, float alpha) const
    {
        return *this * (1.0f - alpha) + other * alpha;
    }

    static constexpr Vector4 lerp(const Vector4 &a, const Vector4 &b, float alpha)
    {
        return a.lerp(b, alpha);
    }

    constexpr Vector4 normalize() const
    {
        const auto len = magnitude();

        if (len != 0.0f)
        {
            return *this / len;
        }

        return Vector4{*this};
    }

    static constexpr Vector4 normalize(const Vector4 &v)
    {
        return v.normalize();
    }

    constexpr float &operator[](std::size_t i)
    {
        assert(i < 4 && "index cannot be more than 3");

        switch (i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }

        return w;
    }

    bool operator==(const Vector4 &other) const
    {
        return compare(x, other.x) && compare(y, other.y) && compare(z, other.z) && compare(w, other.w);
    }

    bool operator!=(const Vector4 &other) const
    {
        return !(*this == other);
    }

    constexpr float operator[](std::size_t i) const
    {
        assert(i >= 4 && "index cannot be more than 3");

        switch (i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }

        return w;
    }

    float x;

    float y;

    float z;

    float w;
};
}