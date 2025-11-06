#include "Plane.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include "../ray/Ray.hpp"
#include "../sphere/Sphere.hpp"

static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

bool Plane::Intersect(const Vec3 &o, const Vec3 &d, float &out_t)
{
    float denom = dot(normal, d);
    if (std::fabs(denom) > 1e-6f) {
        float t = dot(point - o, normal) / denom;
        if (t >= 1e-4f) { // Use a small epsilon to avoid self-intersection
            out_t = t;
            return true;
        }
    }
    return false;
}

void Plane::Draw(Image& /*img*/) const 
{
    std::cout << "Drawing";
 
    // This method is no longer needed for raytracing.
    // It's part of the Shape interface, so we provide an empty implementation.
}
