#pragma once
#include <algorithm>
#include <catedu/core/math/math.hpp>
#include <cmath>
#include <cstdint>

struct HSV
{
    float h, s, v;
};

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    constexpr Color() : r(0), g(0), b(0), a(0)
    {
    }
    constexpr Color(uint32_t hex) : Color(Color::hex(hex))
    {
    }
    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : Color(Color::rgb(r, g, b))
    {
    }
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : Color(Color::rgba(r, g, b, a))
    {
    }
    constexpr Color(Vector4 v)
        : Color(Color::rgba(v.x * 255, v.y * 255, v.z * 255, v.w * 255))
    {
    }

    constexpr Color darken(float amount) const
    {
        return Color::rgb(r * amount, g * amount, b * amount);
    }

    /// @param hex hex color 0xRRGGBBAA
    constexpr static Vector4 hex2v4(uint32_t hex);
    constexpr static Color hex(uint32_t hex);
    constexpr static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    constexpr static Color rgb(uint8_t r, uint8_t g, uint8_t b);
    constexpr static Color hsv(HSV hsv);
    constexpr HSV to_hsv() const;
    constexpr Vector4 to_vector4() const;
};

constexpr inline Vector4 Color::hex2v4(uint32_t hex)
{
    return {((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f, (hex & 0xFF) / 255.0f};
}

constexpr inline Color Color::hex(uint32_t hex)
{
    return rgba((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF,
                (hex) & 0xFF);
}

constexpr inline Color Color::rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

constexpr inline Color Color::rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return rgba(r, g, b, 255);
}

constexpr inline Color Color::hsv(HSV hsv)
{
    // Yes, `abs` isn't constexpr in C++17, I don't know what they were
    // thinking.
    constexpr auto abs = [](float x) { return x < 0 ? -x : x; };

    float c = hsv.v * hsv.s;
    float x = c * (1 - abs(fmod(hsv.h / 60, 2) - 1));
    float m = hsv.v - c;

    float r = 0, g = 0, b = 0;
    if (hsv.h < 60)
    {
        r = c;
        g = x;
        b = 0;
    }
    else if (hsv.h < 120)
    {
        r = x;
        g = c;
        b = 0;
    }
    else if (hsv.h < 180)
    {
        r = 0;
        g = c;
        b = x;
    }
    else if (hsv.h < 240)
    {
        r = 0;
        g = x;
        b = c;
    }
    else if (hsv.h < 300)
    {
        r = x;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = x;
    }

    return rgba((r + m) * 255, (g + m) * 255, (b + m) * 255, 255);
}

constexpr inline HSV Color::to_hsv() const
{
    float r = this->r / 255.0f;
    float g = this->g / 255.0f;
    float b = this->b / 255.0f;

    float max = std::max(std::max(r, g), b);
    float min = std::min(std::min(r, g), b);
    float delta = max - min;

    float h = 0;
    if (delta != 0)
    {
        if (max == r)
        {
            h = 60 * fmod((g - b) / delta, 6);
        }
        else if (max == g)
        {
            h = 60 * ((b - r) / delta + 2);
        }
        else
        {
            h = 60 * ((r - g) / delta + 4);
        }
    }

    float s = max == 0 ? 0 : delta / max;
    float v = max;

    return {h, s, v};
}

constexpr inline Vector4 Color::to_vector4() const
{
    return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}
