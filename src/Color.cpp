#include <iostream>
#include <algorithm>
#include "MathUtils.hpp"

class Color {
private:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

public:
    Color() = default;
    Color(float r_, float g_, float b_) : r(MathUtils::clamp01(r_)), g(MathUtils::clamp01(g_)), b(MathUtils::clamp01(b_)) {}
    ~Color() = default;

    float R() const { return r; }
    float G() const { return g; }
    float B() const { return b; }

    Color operator+(Color const& col) const {
        return Color(MathUtils::clamp01(r + col.r), MathUtils::clamp01(g + col.g), MathUtils::clamp01(b + col.b));
    }
    Color& operator=(Color const& col) = default;

    friend std::ostream& operator<<(std::ostream& os, Color const& c) {
        os << "Color(" << c.r << ", " << c.g << ", " << c.b << ")";
        return os;
    }
};