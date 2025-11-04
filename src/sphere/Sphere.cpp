#include "Sphere.hpp"
#include <algorithm>
#include <cmath>

/**
 * Assure qu’une valeur reste dans l’intervalle [0…1]
 * Permet d'éviter que les composantes de couleur deviennent négatives ou supérieures à 1
 */
static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

Sphere::Sphere(const Vec3& center, float radius, const Color& color, float reflectivity)
    : _center(center), _radius(radius), _color(color), _reflectivity(reflectivity) {
}

// Ray-sphere intersection implementation.
// Solves: || o + t*d - center ||^2 = r^2, with d assumed normalized.
// Quadratic: t^2 + 2*dot(d,oc)*t + dot(oc,oc)-r^2 = 0, where oc = o - center
bool Sphere::Intersect(const Vec3 &o, const Vec3 &d, float &out_t)
{
    Vec3 oc = o - _center;
    float b = 2.0f * dot(d, oc);
    float c = dot(oc, oc) - _radius * _radius;
    float disc = b * b - 4.0f * c;

    if (disc < 0.0f)
        return false;

    float sq = std::sqrt(disc);
    float t0 = (-b - sq) / 2.0f;
    float t1 = (-b + sq) / 2.0f;
    float t = t0;

    if (t < 0.0f)
        t = t1;
    if (t < 0.0f)
        return false;

    out_t = t;
    return true;
}

Color Sphere::GetShadedColor(const Vec3& hitPoint) const {
    // Calculer la normale au point d'intersection
    Vec3 normal = normalize(hitPoint - _center);

    // Lumière venant du haut : direction (0, -1, 0.3) normalisée
    Vec3 lightDir = normalize(Vec3(0.0f, -1.0f, 0.3f));

    // Éclairage ambiant + diffus (Lambert)
    const float ambient = 0.2f;
    float diff = std::max(0.0f, dot(normal, lightDir));
    float intensity = clamp01(ambient + 0.8f * diff);

    return Color(
        clamp01(_color.R() * intensity),
        clamp01(_color.G() * intensity),
        clamp01(_color.B() * intensity)
    );
}