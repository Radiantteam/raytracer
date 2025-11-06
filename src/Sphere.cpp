#include "../include/Sphere.hpp"
#include <algorithm>
#include <cmath>

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

    // View direction (toward camera, assuming camera is far behind in -Z)
    // For a more accurate view vector, this should come from the ray direction
    Vec3 viewDir = normalize(Vec3(0.0f, 0.0f, -1.0f));

    // === DIFFUSE (Lambert) ===
    // Diffuse shading - how much surface faces the light
    const float ambient = 0.15f;  // Reduced ambient for more dramatic metallic look
    float diff = std::max(0.0f, dot(normal, lightDir));

    // === SPECULAR (Blinn-Phong for metallic surfaces) ===
    // Calculate halfway vector between light and view directions
    Vec3 halfwayDir = normalize(lightDir + viewDir);

    // Specular intensity using Blinn-Phong model
    // Higher shininess = tighter, more mirror-like highlight (typical for metals: 32-128)
    const float shininess = 64.0f;  // Metallic surfaces have high shininess
    float spec = std::pow(std::max(0.0f, dot(normal, halfwayDir)), shininess);

    // Metals have strong specular highlights (0.6-0.9 typical)
    const float specularStrength = 0.7f;

    // === METALLIC COLOR MIXING ===
    // For metals, the diffuse color is tinted and specular takes on the metal's color
    // DO NOT clamp intermediate values - let Color constructor handle final clamping
    // Premature clamping causes visible banding/posterization artifacts
    float diffuseIntensity = ambient + 0.5f * diff;  // Reduced diffuse contribution
    float specularIntensity = specularStrength * spec;

    // Metals reflect their base color in specular highlights
    // Color constructor will clamp to [0,1], preserving smooth gradients
    return Color(
        _color.R() * diffuseIntensity + _color.R() * specularIntensity,
        _color.G() * diffuseIntensity + _color.G() * specularIntensity,
        _color.B() * diffuseIntensity + _color.B() * specularIntensity
    );
}