/**
 * ============================================================================
 * Ray.cpp - Ray Tracing Core Logic
 * ============================================================================
 *
 * WHAT THIS FILE DOES:
 * This file implements the core ray tracing algorithm that simulates how light
 * travels through a 3D scene. It traces rays (imaginary light beams) from the
 * camera through each pixel and determines what color that pixel should be
 * based on what the ray hits in the scene.
 *
 * KEY CONCEPTS FOR BEGINNERS:
 *
 * 1. RAY: A mathematical line starting at a point (origin) and going in a
 *    direction. Think of it as a laser beam shot from the camera through a pixel.
 *
 * 2. INTERSECTION: When a ray hits an object in the scene (sphere, plane, etc.)
 *
 * 3. RECURSIVE RAY TRACING: When a ray hits a reflective surface, we cast
 *    another ray (reflection ray) to see what that surface reflects. This
 *    continues up to a maximum depth to prevent infinite loops.
 *
 * ALGORITHMS USED:
 *
 * 1. **Ray-Object Intersection Testing**
 *    - For each ray, we test if it hits any object in the scene
 *    - We keep track of the CLOSEST hit (the object we see first)
 *    - Algorithm: Linear search through all objects, comparing distances
 *
 * 2. **Fresnel Effect (Schlick's Approximation)**
 *    - Models how reflectivity changes based on viewing angle
 *    - Looking at a surface straight-on: less reflective
 *    - Looking at a surface at grazing angles: more reflective (like a shiny car)
 *    - Formula: R(θ) = R₀ + (1 - R₀)(1 - cos(θ))⁵
 *    - This creates realistic metallic appearance
 *
 * 3. **Recursive Reflection Tracing**
 *    - When ray hits reflective surface, compute reflection direction
 *    - Cast new ray in reflection direction (recursive call)
 *    - Mix surface color with reflected color based on reflectivity
 *    - Depth parameter prevents infinite recursion (max depth = 5)
 *
 * 4. **Surface Shading**
 *    - Spheres: Use Blinn-Phong shading with Fresnel effect
 *    - Planes: Checkerboard pattern with optional reflections
 *
 * HOW IT WORKS (Step by step):
 *
 * Step 1: Ray is cast from camera through a pixel
 * Step 2: Test intersection with ALL objects in scene
 * Step 3: Find the CLOSEST object that was hit
 * Step 4: Calculate hit point (where ray touched the object)
 * Step 5: Get surface color at hit point (with lighting/shading)
 * Step 6: If surface is reflective:
 *         - Apply Fresnel effect based on view angle
 *         - Cast reflection ray (recursive call with depth-1)
 *         - Mix surface color with reflection color
 * Step 7: Return final color for this pixel
 *
 * IMPORTANT DETAILS:
 *
 * - Small epsilon offset (1e-4f): When casting reflection rays, we move the
 *   origin slightly away from the surface to prevent "shadow acne" (self-
 *   intersection artifacts where a surface reflects itself incorrectly)
 *
 * - Color mixing uses raw floats: To avoid premature clamping that causes
 *   artifacts, we work with raw float values and only create the final Color
 *   object at the end
 *
 * - dynamic_cast: Used to determine which type of shape was hit (Sphere or
 *   Plane) so we can apply the appropriate shading model
 */

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
