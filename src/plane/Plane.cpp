#include "Plane.hpp"
#include <cmath>
#include <algorithm>

static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

float Plane::Intersect(const Vec3& rayOrigin, const Vec3& rayDir) const {
    float denom = dot(normal, rayDir);
    if (std::fabs(denom) < 1e-6f) return -1.0f; // parallèle
    float t = dot(point - rayOrigin, normal) / denom;
    return (t >= 0.0f) ? t : -1.0f;
}

void Plane::Draw(Image& img, const Plane& plane) {
    const int W = img.GetWidth();
    const int H = img.GetHeight();

    Vec3 camOrigin = {0.0f, 0.0f, -500.0f}; // position de caméra
    float viewportZ = 1.0f;

    Vec3 lightDir = normalize(Vec3{-0.2f, -0.5f, 1.0f});
    const float ambient = 0.1f;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            // Coordonnées normalisées [-1,1]
            float px = (2.0f * x / W - 1.0f) * (float(W) / H);
            float py = 1.0f - 2.0f * y / H;

            Vec3 rayDir = normalize(Vec3{px, py, viewportZ});

            float t = plane.Intersect(camOrigin, rayDir);
            if (t > 0.0f) {
                Vec3 hitPoint = camOrigin + rayDir * t;
                Vec3 N = plane.normal;
                Vec3 L = normalize(-lightDir);
                Vec3 V = normalize(-rayDir);
                Vec3 R = reflect(-L, N);

                // Lambert + Phong
                float diff = std::max(0.0f, dot(N, L));
                float spec = std::pow(std::max(0.0f, dot(R, V)), 32.0f);
                float shade = clamp01(ambient + 0.8f * diff + 0.4f * spec);

                img.SetPixel(
                    x, y,
                    Color(
                        plane.baseColor.R() * shade,
                        plane.baseColor.G() * shade,
                        plane.baseColor.B() * shade
                    )
                );
            }
        }
    }
}
