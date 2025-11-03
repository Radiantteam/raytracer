#include "./Cube.hpp"
#include <algorithm>
#include <cmath>

/**
 * Assure qu’une valeur reste dans l’intervalle [0…1]
 * Permet d'éviter que les composantes de couleur deviennent négatives ou supérieures à 1
 */
static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

Cube::Cube(const Vec3 &center, float size, const Color &color)
    : _center(center), _size(size), _color(color)
{
}

void Cube::Draw(Image &img) const
{
    const int W = img.GetWidth();
    const int H = img.GetHeight();
    const int cx = static_cast<int>(_center.x);
    const int cy = static_cast<int>(_center.y);

    // ambient → quantité minimale de lumière présente même dans l’ombre
    const float ambient = 0.12f;

    // direction de la lumière, normalisé pour obtenir un calcul de lumière cohérent
    const Vec3 lightDir = normalize(Vec3{0.0f, 0.0f, 1.0f});

    // Le cube est plat sur l'axe Z, donc sa normale est constante.
    const Vec3 n = {0.0f, 0.0f, 1.0f};

    const float diff = std::max(0.0f, dot(n, lightDir));
    const float shade = clamp01(ambient + 0.8f * diff);

    const Color finalColor(
        clamp01(_color.R() * shade),
        clamp01(_color.G() * shade),
        clamp01(_color.B() * shade));

    const int halfSize = static_cast<int>(_size / 2.0f);
    const int x0 = std::max(0, cx - halfSize);
    const int x1 = std::min(W - 1, cx + halfSize);
    const int y0 = std::max(0, cy - halfSize);
    const int y1 = std::min(H - 1, cy + halfSize);

    for (int y = y0; y <= y1; ++y)
    {
        for (int x = x0; x <= x1; ++x)
        {
            img.SetPixel(x, y, finalColor);
        }
    }
}

bool Cube::Intersect(const Vec3 &o, const Vec3 &d, float &out_t)
{
    Vec3 half = Vec3{_size / 2, _size / 2, _size / 2};
    Vec3 min = _center - half;
    Vec3 max = _center + half;

    float tmin = (min.x - o.x) / d.x;
    float tmax = (max.x - o.x) / d.x;
    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (min.y - o.y) / d.y;
    float tymax = (max.y - o.y) / d.y;
    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (min.z - o.z) / d.z;
    float tzmax = (max.z - o.z) / d.z;
    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    out_t = tmin >= 0 ? tmin : tmax;
    return out_t >= 0;
}
