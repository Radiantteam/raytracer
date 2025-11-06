#pragma once

#include <vector>
#include <memory>
#include "Vec3.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "Shape.hpp"

class Plane : public Shape {
public:
    Vec3 point;   // Un point sur le plan
    Vec3 normal;  // Normale (doit être normalisée)
    float reflectivity;

    Plane(const Vec3& p, const Vec3& n, float reflect = 0.0f)
        : point(p), normal(normalize(n)), reflectivity(reflect) {}

    bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t) override;

    // The Draw method is now part of the Shape interface, but not used for raytracing
    void Draw(Image& img) const override;
};
