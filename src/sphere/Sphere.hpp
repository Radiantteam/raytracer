#pragma once

#include "../Shape.hpp"
#include "../vec/Vec3.hpp"
#include "image/Image.hpp"

// Dessine une sphère centrée (cx, cy), rayon en pixels, teintée par baseColor.
// Éclairage minimal : ambiant + Lambert
class Sphere : public Shape {
public:
    Sphere(const Vec3& center, float radius, const Color& color);

    void Draw(Image& img) const override;

private:
    Vec3 _center;
    float _radius;
    Color _color;
};