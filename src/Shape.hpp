#pragma once

#include "image/Image.hpp"
#include "vec/Vec3.hpp"

class Shape
{
public:
    virtual ~Shape() = default;              // Virtual destructor for polymorphism
    virtual void Draw(Image &img) const = 0; // Pure virtual function

    // Méthode d’intersection générique (à spécialiser par forme)
    virtual bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t);
};
