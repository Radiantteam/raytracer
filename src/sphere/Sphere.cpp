#include "Sphere.hpp"
#include <algorithm>
#include <cmath>

/**
 * Assure qu’une valeur reste dans l’intervalle [0…1]
 * Permet d'éviter que les composantes de couleur deviennent négatives ou supérieures à 1
 */
static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

Sphere::Sphere(const Vec3& center, float radius, const Color& color)
    : _center(center), _radius(radius), _color(color) {
}

void Sphere::Draw(Image& img) const {
    const int W = img.GetWidth();
    const int H = img.GetHeight();

    const int cx = static_cast<int>(_center.x);
    const int cy = static_cast<int>(_center.y);
    const float radius = _radius;

    // ambient → quantité minimale de lumière présente même dans l’ombre
    const float ambient = 0.12f;

    // direction de la lumière, normalisé pour obtenir un calcul de lumière cohérent
    const Vec3  lightDir = normalize(Vec3{0.0f, 0.0f, 1.0f});

    // Définit une zone rectangulaire autour de la sphère à dessiner
    const int x0 = std::max(0,     static_cast<int>(std::floor(cx - radius)));
    const int x1 = std::min(W - 1, static_cast<int>(std::ceil (cx + radius)));
    const int y0 = std::max(0,     static_cast<int>(std::floor(cy - radius)));
    const int y1 = std::min(H - 1, static_cast<int>(std::ceil (cy + radius)));

    const float r2 = radius * radius;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const float dx = float(x) - float(cx);
            const float dy = float(y) - float(cy);
            const float d2 = dx*dx + dy*dy;

            // Ignorer les pixels en dehors du cercle
            if (d2 > r2) continue;

            // Reconstruction locale du z et de la normale
            const float dz = std::sqrt(std::max(0.0f, r2 - d2));
            const Vec3 n = normalize(Vec3{dx, dy, dz});

            //La réflectance lambertienne est la propriété qui définit une surface idéale « mate » ou
            // à réflexion diffuse. La luminosité apparente d'une surface lambertienne
            // pour un observateur est la même quel que soit l'angle de vue de l'observateur.
            const float diff = std::max(0.0f, dot(n, lightDir));
            const float shade = clamp01(ambient + 0.8f * diff);

            img.SetPixel(
                x, y,
                Color(
                    clamp01(_color.R() * shade),
                    clamp01(_color.G() * shade),
                    clamp01(_color.B() * shade)
                )
            );
        }
    }
}