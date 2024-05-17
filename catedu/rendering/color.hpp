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

    /// @param hex hex color 0xRRGGBBAA
    static Vector4 hex2v4(uint32_t hex);
    static Color hex(uint32_t hex);
    static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static Color rgb(uint8_t r, uint8_t g, uint8_t b);
    static Color hsv(HSV hsv);
    HSV to_hsv() const;
    Vector4 to_vector4() const;
};

inline Vector4 Color::hex2v4(uint32_t hex)
{
    return {((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f, (hex & 0xFF) / 255.0f};
}

inline Color Color::hex(uint32_t hex)
{
    return rgba((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF,
                (hex) & 0xFF);
}

inline Color Color::rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

inline Color Color::rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return rgba(r, g, b, 255);
}

inline Color Color::hsv(HSV hsv)
{
    float c = hsv.v * hsv.s;
    float x = c * (1 - abs(fmod(hsv.h / 60, 2) - 1));
    float m = hsv.v - c;

    float r, g, b;
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

inline HSV Color::to_hsv() const
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

inline Vector4 Color::to_vector4() const
{
    return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
}
