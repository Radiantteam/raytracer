#include "Ray.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cube.hpp"
#include "Vec3.hpp"
#include <algorithm>

Ray::Ray(const Vec3& origin, const Vec3& direction)
    : _origin(origin), _direction(direction) {
}

// Schlick's approximation of Fresnel reflectance for metals
// Returns reflectivity factor based on view angle (0 = face-on, 1 = grazing angle)
// For metallic surfaces, reflectivity increases dramatically at grazing angles
static float FresnelSchlick(float cosTheta, float baseReflectivity) {
    // Schlick's approximation: R(θ) = R₀ + (1 - R₀)(1 - cos(θ))⁵
    // where R₀ is the base reflectivity and θ is the angle between view and normal
    float r0 = baseReflectivity;
    float oneMinusCos = 1.0f - cosTheta;
    float oneMinusCos5 = oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;
    return r0 + (1.0f - r0) * oneMinusCos5;
}

Color Ray::TraceScene(const std::vector<std::unique_ptr<Shape>>& scene, int depth) const {
    Color defaultColor = Color(0.5f,0.4f,0.5f);

    if (depth <= 0) return defaultColor;

    float closest_t = 1e30f;
    const Shape* hit_shape = nullptr;

    // Trouver la forme la plus proche intersectée
    for (const auto& shape : scene) {
        float t;
        if (shape->Intersect(_origin, _direction, t)) {
            if (t < closest_t) {
                closest_t = t;
                hit_shape = shape.get();
            }
        }
    }

    if (! hit_shape) {
        return defaultColor;
    }

    // Si intersection trouvée, calculer la couleur avec ombrage
    Vec3 hitPoint = PointAt(closest_t);

    if (const Sphere* hit_sphere = dynamic_cast<const Sphere*>(hit_shape)) {
        Color surfaceColor = hit_sphere->GetShadedColor(hitPoint);
        float baseReflectivity = hit_sphere->GetReflectivity();

        if (baseReflectivity > 0.0f) {
            Vec3 normal = normalize(hitPoint - hit_sphere->GetCenter());

            // Apply Fresnel effect: reflectivity increases at grazing angles
            // This creates realistic metallic appearance where edges are more reflective
            float cosTheta = std::abs(dot(normalize(-GetDirection()), normal));
            float fresnelReflectivity = FresnelSchlick(cosTheta, baseReflectivity);

            // Cap maximum reflectivity to ensure sphere surface remains visible
            // Even at 1.0f base reflectivity, we keep at least 10% of surface color
            // This allows the sphere's color and shading to remain visible
            const float maxReflectivity = 0.90f;
            fresnelReflectivity = std::min(fresnelReflectivity, maxReflectivity);

            // Cast reflection ray
            Vec3 reflectDir = reflect(GetDirection(), normal);
            Ray reflectedRay(hitPoint + normal * 1e-4f, reflectDir);
            Color reflectionColor = reflectedRay.TraceScene(scene, depth - 1);

            // Mix surface color and reflection using raw floats to avoid premature clamping
            // Color class clamps on multiplication and addition, which breaks color mixing
            // Working with raw floats preserves precision and eliminates pixelization artifacts
            float surfaceWeight = 1.0f - fresnelReflectivity;
            float reflectionWeight = fresnelReflectivity;

            return Color(
                surfaceColor.R() * surfaceWeight + reflectionColor.R() * reflectionWeight,
                surfaceColor.G() * surfaceWeight + reflectionColor.G() * reflectionWeight,
                surfaceColor.B() * surfaceWeight + reflectionColor.B() * reflectionWeight
            );
        }
        return surfaceColor;
    }

    // === Cube shading and reflection ===
    if (const Cube *hit_cube = dynamic_cast<const Cube *>(hit_shape))
    {
        Color surfaceColor = hit_cube->GetShadedColor(hitPoint);
        float baseReflectivity = hit_cube->GetReflectivity();

        if (baseReflectivity > 0.0f)
        {
            // Compute cube face normal - SAME METHOD AS GetShadedColor
            Vec3 half = Vec3{hit_cube->GetSize() / 2.0f, hit_cube->GetSize() / 2.0f, hit_cube->GetSize() / 2.0f};
            Vec3 local = hitPoint - hit_cube->GetCenter();

            float distX = half.x - std::abs(local.x);
            float distY = half.y - std::abs(local.y);
            float distZ = half.z - std::abs(local.z);

            Vec3 normal;
            if (distX < distY && distX < distZ) {
                normal = Vec3((local.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f);
            } else if (distY < distZ) {
                normal = Vec3(0.0f, (local.y > 0) ? 1.0f : -1.0f, 0.0f);
            } else {
                normal = Vec3(0.0f, 0.0f, (local.z > 0) ? 1.0f : -1.0f);
            }

            float cosTheta = std::abs(dot(normalize(-GetDirection()), normal));
            float fresnelReflectivity = std::min(FresnelSchlick(cosTheta, baseReflectivity), 0.9f);

            Vec3 reflectDir = reflect(GetDirection(), normal);
            Ray reflectedRay(hitPoint + normal * 1e-4f, reflectDir);
            Color reflectionColor = reflectedRay.TraceScene(scene, depth - 1);

            float surfaceWeight = 1.0f - fresnelReflectivity;
            float reflectionWeight = fresnelReflectivity;

            return Color(
                surfaceColor.R() * surfaceWeight + reflectionColor.R() * reflectionWeight,
                surfaceColor.G() * surfaceWeight + reflectionColor.G() * reflectionWeight,
                surfaceColor.B() * surfaceWeight + reflectionColor.B() * reflectionWeight);
        }

        return surfaceColor;
    }

    if (const Plane* hit_plane = dynamic_cast<const Plane*>(hit_shape)) {
        // --- Checkerboard Pattern for the plane ---
        float scale = 0.001f;
        int check = (static_cast<int>(std::floor(hitPoint.x * scale)) + static_cast<int>(std::floor(hitPoint.z * scale))) & 1;
        Color surfaceColor = check ? Color(1.0f, 1.0f, 1.0f) : Color(0.2f, 0.2f, 0.2f);

        float reflectivity = hit_plane->reflectivity;
        if (reflectivity > 0.0f) {
            Vec3 normal = hit_plane->normal;
            Vec3 reflectDir = reflect(GetDirection(), normal);
            Ray reflectedRay(hitPoint + normal * 1e-4f, reflectDir);
            Color reflectionColor = reflectedRay.TraceScene(scene, depth - 1);

            // Mix surface color and reflection using raw floats to avoid premature clamping
            float surfaceWeight = 1.0f - reflectivity;
            float reflectionWeight = reflectivity;

            return Color(
                surfaceColor.R() * surfaceWeight + reflectionColor.R() * reflectionWeight,
                surfaceColor.G() * surfaceWeight + reflectionColor.G() * reflectionWeight,
                surfaceColor.B() * surfaceWeight + reflectionColor.B() * reflectionWeight
            );
        }
        return surfaceColor;
    }

    return defaultColor;
}
