#pragma once

#include "Image.hpp"
#include "../vec/Vec3.hpp"

// Dessine une sphère centrée (cx, cy), rayon en pixels, teintée par baseColor.
// Éclairage minimal : ambiant + Lambert
class Sphere {
public:
    static void Draw(
        Image& img,
        int cx, int cy,
        float radius,
        const Color& baseColor
    );
};