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

    // caméra : on relève un peu en Y pour que l’horizon descende
    Vec3 camOrigin = {0.0f, 0.6f, -800.0f};
    float viewportZ = 1.0f;

    // EXACTEMENT comme Sphere.cpp
    const float ambient = 0.12f;
    const Vec3  lightDir = normalize(Vec3{-0.1f, -2.0f, 0.5f});

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            float px = (2.0f * x / W - 1.0f) * (float(W) / H);
            float py = 1.0f - 2.0f * y / H;

            Vec3 rayDir = normalize(Vec3{px, py, viewportZ});
            float t = plane.Intersect(camOrigin, rayDir);

            // juste "devant la caméra"
            if (t > 0.0f) {
                Vec3 hitPoint = camOrigin + rayDir * t;
                Vec3 N = plane.normal; // Normal is constant for a plane

                // --- Checkerboard Pattern ---
                const Color color1 = Color(1.0f, 1.0f, 1.0f); // White
                const Color color2 = Color(0.1f, 0.1f, 0.1f); // Dark Gray/Black
                float scale = 0.5f;
                int check = (static_cast<int>(std::floor(hitPoint.x * scale)) + static_cast<int>(std::floor(hitPoint.z * scale))) & 1;
                Color finalColor = check ? color1 : color2;

                float diff = std::max(0.0f, dot(N, lightDir));
                float shade = clamp01(ambient + 0.8f * diff);

                img.SetPixel(
                    x, y,
                    Color(
                        finalColor.R() * shade,
                        finalColor.G() * shade,
                        finalColor.B() * shade
                    )
                );
            }
        }
    }
}
