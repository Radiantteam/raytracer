#pragma once

#include "Image.hpp"
#include "../vec/Vec3.hpp"

// Dessine une sphère centrée (cx, cy), rayon en pixels, teintée par baseColor.
// Éclairage minimal : ambiant + Lambert
class Sphere
{
public:
    static void Draw(
        Image &img,
        int cx, int cy,
        float radius,
        const Color &baseColor);

    // Ray-sphere intersection helper:
    // returns true if ray (o + t*d) intersects sphere with given center/radius,
    // and writes nearest positive t to out_t.
    static bool Intersect(const Vec3 &center, float radius,
                          const Vec3 &o, const Vec3 &d, float &out_t);
};
