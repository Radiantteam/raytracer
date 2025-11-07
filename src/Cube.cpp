#include <algorithm>
#include <cmath>
#include "Cube.hpp"

Cube::Cube(const Vec3 &center, float size, const Color &color, float reflectivity)
    : _center(center), _size(size), _color(color)
{
}

bool Cube::Intersect(const Vec3 &o, const Vec3 &d, float &out_t)
{
    const float EPS = 1e-8f;
    Vec3 half = Vec3{_size / 2.0f, _size / 2.0f, _size / 2.0f};
    Vec3 mn = _center - half;
    Vec3 mx = _center + half;

    float tmin = -INFINITY;
    float tmax = INFINITY;

    auto test_axis = [&](float origin, float dir, float minv, float maxv) -> bool
    {
        if (std::abs(dir) < EPS)
        {
            // Ray parallel to this axis: must be inside slab
            if (origin < minv || origin > maxv)
                return false;
            return true; // no change to tmin/tmax
        }
        else
        {
            float inv = 1.0f / dir;
            float t1 = (minv - origin) * inv;
            float t2 = (maxv - origin) * inv;
            if (t1 > t2)
                std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            return tmax >= tmin;
        }
    };

    if (!test_axis(o.x, d.x, mn.x, mx.x))
        return false;
    if (!test_axis(o.y, d.y, mn.y, mx.y))
        return false;
    if (!test_axis(o.z, d.z, mn.z, mx.z))
        return false;

    float t_hit = (tmin >= 0.0f) ? tmin : tmax;
    if (t_hit < 0.0f)
        return false;

    out_t = t_hit;
    return true;
}

Color Cube::GetShadedColor(const Vec3& hitPoint) const {
    // === Compute surface normal based on which face was hit ===
    Vec3 half = Vec3{_size / 2.0f, _size / 2.0f, _size / 2.0f};
    Vec3 local = hitPoint - _center;

    // Calculate distances from each face
    float distX = half.x - std::abs(local.x);
    float distY = half.y - std::abs(local.y);
    float distZ = half.z - std::abs(local.z);

    // The face with minimum distance is the one that was hit
    Vec3 normal;
    if (distX < distY && distX < distZ) {
        normal = Vec3((local.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f);
    } else if (distY < distZ) {
        normal = Vec3(0.0f, (local.y > 0) ? 1.0f : -1.0f, 0.0f);
    } else {
        normal = Vec3(0.0f, 0.0f, (local.z > 0) ? 1.0f : -1.0f);
    }

    // === Same lighting model as Sphere ===
    Vec3 lightDir = normalize(Vec3(0.0f, -1.0f, 0.3f));
    Vec3 viewDir  = normalize(Vec3(0.0f, 0.0f, -1.0f));

    const float ambient = 0.15f;
    float diff = std::max(0.0f, dot(normal, lightDir));

    Vec3 halfwayDir = normalize(lightDir + viewDir);
    const float shininess = 64.0f;
    float spec = std::pow(std::max(0.0f, dot(normal, halfwayDir)), shininess);
    const float specularStrength = 0.7f;

    float diffuseIntensity = ambient + 0.5f * diff;
    float specularIntensity = specularStrength * spec;

    return Color(
        _color.R() * diffuseIntensity + _color.R() * specularIntensity,
        _color.G() * diffuseIntensity + _color.G() * specularIntensity,
        _color.B() * diffuseIntensity + _color.B() * specularIntensity
    );
}