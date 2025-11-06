#include <cmath>
#include <algorithm>
#include <limits>
#include "Plane.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"

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