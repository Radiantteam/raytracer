#pragma once

#include "vec/Vec3.hpp"
#include "color/Color.hpp"
#include "image/Image.hpp"

class Plane
{
public:
    Vec3 point;  // Un point sur le plan
    Vec3 normal; // Normale (doit être normalisée)
    Color baseColor;

    Plane(const Vec3 &p, const Vec3 &n, const Color &c)
        : point(p), normal(normalize(n)), baseColor(c) {}

    // Renvoie la distance t de l'intersection ray/plan, ou -1 si aucun hit
    float Intersect(const Vec3 &rayOrigin, const Vec3 &rayDir) const;

    // Dessine le plan dans l’image (simple vue orthographique)
    static void Draw(Image &img, const Plane &plane);
};
