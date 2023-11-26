#pragma once

#include "utils/public/utils.h"

#include <cmath>

namespace muse
{
class Vector2
{
  public:
    constexpr Vector2()
        : Vector2(0.0f)
    {
    }

    constexpr Vector2(float xy)
        : Vector2(xy, xy)
    {
    }

    constexpr Vector2(float x, float y)
        : x(x)
        , y(y)
    {
    }

    constexpr Vector2 &operator*=(const Vector2 &v)
    {
        x *= v.x;
        y *= v.y;

        return *this;
    }

    constexpr Vector2 operator*(const Vector2 &v) const
    {
        return Vector2(*this) *= v;
    }

    constexpr Vector2 &operator*=(float scale)
    {
        x *= scale;
        y *= scale;

        return *this;
    }

    constexpr Vector2 operator*(float scale) const
    {
        return Vector2(*this) *= scale;
    }

    constexpr Vector2 &operator/=(const Vector2 &v)
    {
        x /= v.x;
        y /= v.y;

        return *this;
    }

    constexpr Vector2 operator/(const Vector2 &v) const
    {
        return Vector2(*this) /= v;
    }

    constexpr Vector2 &operator/=(float scale)
    {
        x /= scale;
        y /= scale;

        return *this;
    }

    constexpr Vector2 operator/(float scale) const
    {
        return Vector2(*this) /= scale;
    }

    constexpr Vector2 &operator+=(const Vector2 &v)
    {
        x += v.x;
        y += v.y;

        return *this;
    }

    constexpr Vector2 operator+(const Vector2 &v) const
    {
        return Vector2(*this) += v;
    }

    constexpr Vector2 &operator+=(float scale)
    {
        x += scale;
        y += scale;

        return *this;
    }

    constexpr Vector2 operator+(float scale) const
    {
        return Vector2(*this) += scale;
    }

    constexpr Vector2 &operator-=(const Vector2 &v)
    {
        x -= v.x;
        y -= v.y;

        return *this;
    }

    constexpr Vector2 operator-(const Vector2 &v) const
    {
        return Vector2(*this) -= v;
    }

    constexpr Vector2 &operator-=(float scale)
    {
        x -= scale;
        y -= scale;

        return *this;
    }

    constexpr Vector2 operator-(float scale) const
    {
        return Vector2(*this) -= scale;
    }

    constexpr Vector2 operator-() const
    {
        return Vector2{-x, -y};
    }

    constexpr float magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    static constexpr float magnitude(const Vector2 &v)
    {
        return v.magnitude();
    }

    constexpr float dot(const Vector2 &other) const
    {
        return x * other.x + y * other.y;
    }

    static constexpr float dot(const Vector2 &a, const Vector2 &b)
    {
        return a.dot(b);
    }

    constexpr Vector2 lerp(const Vector2 &other, float alpha) const
    {
        return *this * (1.0f - alpha) + other * alpha;
    }

    static constexpr Vector2 lerp(const Vector2 &a, const Vector2 &b, float alpha)
    {
        return a.lerp(b, alpha);
    }

    constexpr Vector2 normalize() const
    {
        const auto len = magnitude();

        if (len != 0.0f)
        {
            return *this / len;
        }

        return Vector2{*this};
    }

    static constexpr Vector2 normalize(const Vector2 &v)
    {
        return v.normalize();
    }

    static constexpr float distance(const Vector2 &a, const Vector2 &b)
    {
        return (b - a).magnitude();
    }

    bool operator==(const Vector2 &other) const
    {
        return compare(x, other.x) && compare(y, other.y);
    }

    bool operator!=(const Vector2 &other) const
    {
        return !(*this == other);
    }

    float x;

    float y;
};

}