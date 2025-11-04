#include "Ray.hpp"
#include "../sphere/Sphere.hpp"
#include "../plane/Plane.hpp"
#include "../vec/Vec3.hpp"

Ray::Ray(const Vec3& origin, const Vec3& direction)
    : _origin(origin), _direction(direction) {
}

Color Ray::TraceScene(const std::vector<std::unique_ptr<Shape>>& scene, int depth) const {
    if (depth <= 0) return Color(0,0,0);

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

    // Si intersection trouvée, calculer la couleur avec ombrage
    if (hit_shape) {
        Vec3 hitPoint = PointAt(closest_t);

        if (const Sphere* hit_sphere = dynamic_cast<const Sphere*>(hit_shape)) {
            Color surfaceColor = hit_sphere->GetShadedColor(hitPoint);
            float reflectivity = hit_sphere->GetReflectivity();
            if (reflectivity > 0.0f) {
                Vec3 normal = normalize(hitPoint - hit_sphere->GetCenter());
                Vec3 reflectDir = reflect(GetDirection(), normal);
                Ray reflectedRay(hitPoint + normal * 1e-4f, reflectDir);
                Color reflectionColor = reflectedRay.TraceScene(scene, depth - 1);
                return (surfaceColor * (1.0f - reflectivity)) + (reflectionColor * reflectivity);
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
                return (surfaceColor * (1.0f - reflectivity)) + (reflectionColor * reflectivity);
            }
            return surfaceColor;
        }
    }

    // Sinon retourner couleur de fond (transparente, sera ignorée)
    return Color(0.0f, 0.0f, 0.0f);
}