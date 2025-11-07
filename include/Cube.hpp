#pragma once

#include "Shape.hpp"
#include "Image.hpp"
#include "Vec3.hpp"

// Dessine un cube centré (cx, cy), de côté 'size', teinté par baseColor.
// Éclairage minimal : ambiant + Lambert
class Cube : public Shape
{
public:
    Cube(const Vec3 &center, float size, const Color &color, float reflectivity = 0.0f);

    bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t) override;

    const Color& GetColor() const { return _color; }
    Color GetShadedColor(const Vec3& hitPoint) const;
    const Vec3& GetCenter() const { return _center; }
    float GetReflectivity() const { return _reflectivity; }
    float GetSize() const { return _size; }
private:
    Vec3 _center;
    float _size;
    Color _color;
    float _reflectivity;
};
