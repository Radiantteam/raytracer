#include "Sphere.hpp"
#include <algorithm>
#include <cmath>
#include <random>

/**
 * Assure qu’une valeur reste dans l’intervalle [0…1]
 * Permet d'éviter que les composantes de couleur deviennent négatives ou supérieures à 1
 */
 
static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

Sphere::Sphere(const Vec3& center, float radius, const Color& color, float reflectivity)
    : _center(center), _radius(radius), _color(color), _reflectivity(reflectivity) {
    RandomizeTexture();
}

void Sphere::RandomizeTexture()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> typeDist(0, 3);
    std::uniform_real_distribution<float> reflDist(0.0f, 0.6f);
    std::uniform_real_distribution<float> seedDist(0.0f, 1000.0f);

    _textureType = static_cast<TextureType>(typeDist(gen));
    _reflectivity = reflDist(gen);
    _textureSeed = seedDist(gen);
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
    const float ambient = 0.25f;
    float diff = std::max(0.0f, dot(normal, lightDir));
    float intensity = clamp01(ambient + 0.8f * diff);

    Color base = _color;

    switch (_textureType)
{
    case TextureType::Gradient:
    {
        float h = (hitPoint.y - _center.y) / (_radius * 2.0f);
        h = clamp01(h * 0.5f + 0.5f);
        base = Color(
            _color.R() * (0.3f + 0.7f * h),
            _color.G() * (0.3f + 0.7f * h),
            _color.B() * (0.3f + 0.7f * h)
        );
        break;
    }

    case TextureType::Marble:
    {
        float n = std::sin(hitPoint.x * 0.05f + hitPoint.y * 0.07f + hitPoint.z * 0.03f + _textureSeed);
        float veins = 0.5f + 0.5f * std::sin(n * 20.0f);
        base = Color(
            _color.R() * (0.4f + 0.6f * veins),
            _color.G() * (0.4f + 0.6f * veins),
            _color.B() * (0.4f + 0.6f * veins)
        );
        break;
    }

    case TextureType::Noise:
    {
        float noise = std::sin((hitPoint.x * 0.2f + hitPoint.z * 0.3f + _textureSeed) * 3.5f);
        float pattern = 0.5f + 0.5f * noise;
        base = Color(
            _color.R() * (0.5f + 0.5f * pattern),
            _color.G() * (0.5f + 0.5f * pattern),
            _color.B() * (0.5f + 0.5f * pattern)
        );
        break;
    }

    default:
        break;
}


    return Color(
        clamp01(_color.R() * intensity),
        clamp01(_color.G() * intensity),
        clamp01(_color.B() * intensity)
    );
}