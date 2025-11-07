#include "../include/Sphere.hpp"
#include <algorithm>
#include <cmath>
#include "MathUtils.hpp"
#include <random>

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
    Vec3 normal = normalize(hitPoint - _center);
    Vec3 lightDir = normalize(Vec3(0.0f, -1.0f, 0.3f));
    Vec3 viewDir = normalize(Vec3(0.0f, 0.0f, -1.0f));

    // === BASE LIGHTING ===
    const float ambient = 0.15f;
    float diff = std::max(0.0f, dot(normal, lightDir));
    Vec3 halfwayDir = normalize(lightDir + viewDir);
    const float shininess = 64.0f;
    float spec = std::pow(std::max(0.0f, dot(normal, halfwayDir)), shininess);
    const float specularStrength = 0.7f;

    // === TEXTURE SELECTION ===
    float pattern = 1.0f;
    Vec3 local = hitPoint * 0.02f + Vec3(_textureSeed * 0.001f);

    switch (_textureType)
{
    case TextureType::Gradient:
        // dégradé vertical + contraste
        pattern = MathUtils::clamp01(0.3f + 0.7f * normal.y);
        break;

    case TextureType::Marble:
        // marbre 
        pattern = 0.5f + 0.5f * std::sin(local.x * 6.0f + std::sin(local.y * 4.0f) * 3.0f + _textureSeed);
        pattern = std::pow(pattern, 1.4f); 
        break;

    case TextureType::Noise:
        
        pattern = std::fabs(std::sin(local.x * 4.5f + std::cos(local.z * 3.7f) + local.y * 1.5f + _textureSeed));
        pattern = std::pow(pattern, 0.6f); 
        break;
}

    // === APPLY TEXTURE TO BASE COLOR ===
    float r = _color.R() * pattern;
    float g = _color.G() * pattern;
    float b = _color.B() * pattern;

    // === COMBINE LIGHTING ===
    float diffuseIntensity = ambient + 0.5f * diff;
    float specularIntensity = specularStrength * spec;

    return Color(
        MathUtils::clamp01(r * diffuseIntensity + _color.R() * specularIntensity),
        MathUtils::clamp01(g * diffuseIntensity + _color.G() * specularIntensity),
        MathUtils::clamp01(b * diffuseIntensity + _color.B() * specularIntensity)
    );
}

void Sphere::RandomizeTexture() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distType(0, 2);
    std::uniform_real_distribution<float> distReflect(0.0f, 0.8f);

    _textureType = static_cast<TextureType>(distType(gen));
    _reflectivity = distReflect(gen);
    _textureSeed = static_cast<float>(rd() % 10000);
}