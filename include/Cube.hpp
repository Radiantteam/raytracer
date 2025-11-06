#pragma once

#include "Shape.hpp"
#include "Image.hpp"
#include "Vec3.hpp"

// Dessine un cube centré (cx, cy), de côté 'size', teinté par baseColor.
// Éclairage minimal : ambiant + Lambert
class Cube : public Shape
{
public:
    Cube(const Vec3 &center, float size, const Color &color);

    bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t) override;

private:
    Vec3 _center;
    float _size;
    Color _color;
};
