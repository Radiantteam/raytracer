#pragma once

#include "image/Image.hpp"
#include "vec/Vec3.hpp"

class Shape
{
public:
    // Declare destructor out-of-line so we can provide a single key function
    // (this ensures the vtable/typeinfo are emitted in one translation unit).
    virtual ~Shape();                        // Virtual destructor for polymorphism
    virtual void Draw(Image &img) const = 0; // Pure virtual function

    // Méthode d’intersection générique (à spécialiser par forme)
    // Make it pure virtual so Shape remains abstract and each derived shape
    // must implement its own intersection routine.
    virtual bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t) = 0;

    // virtual bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t, Vec3 &out_normal, Color &out_color) = 0;
};
