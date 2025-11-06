#pragma once

#include <iostream>

class Color {
private:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    static inline float clamp01(float v) {
        return std::max(0.0f, std::min(1.0f, v));
    }

public:
    Color() = default;
    Color(float r_, float g_, float b_) : r(clamp01(r_)), g(clamp01(g_)), b(clamp01(b_)) {}
    ~Color() = default;

    float R() const { return r; }
    float G() const { return g; }
    float B() const { return b; }

    Color operator+(Color const& col) const {
        return Color(clamp01(r + col.r), clamp01(g + col.g), clamp01(b + col.b));
    }
    Color operator*(float scalar) const {
        return Color(clamp01(r * scalar), clamp01(g * scalar), clamp01(b * scalar));
    }
    Color& operator=(Color const& col) = default;

    friend std::ostream& operator<<(std::ostream& os, Color const& c) {
        os << "Color(" << c.r << ", " << c.g << ", " << c.b << ")";
        return os;
    }
};
