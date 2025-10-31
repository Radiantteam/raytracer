#pragma once

#include "../Shape.hpp"
#include "../image/Image.hpp"
#include "../vec/Vec3.hpp"

// Dessine un cube centré (cx, cy), de côté 'size', teinté par baseColor.
// Éclairage minimal : ambiant + Lambert
class Cube : public Shape {
public:
    Cube(const Vec3& center, float size, const Color& color);

    void Draw(Image& img) const override;
private:
    Vec3 _center;
    float _size;
    Color _color;
};