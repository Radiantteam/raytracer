#pragma once

#include <vector>
#include <memory>
#include "Shape.hpp"
#include "Sphere.hpp"
#include "Color.hpp"
#include "Vec3.hpp"

/**
 * Class responsible for generating spheres in the scene
 */
class SphereGenerator {
public:
    /**
     * Generate a collection of spheres arranged horizontally
     * @param count Number of spheres to generate
     * @param width Scene width
     * @param height Scene height
     * @return Vector of unique pointers to sphere shapes
     */
    static std::vector<std::unique_ptr<Shape>> Generate(int count, int width, int height);

private:
    static constexpr float SPHERE_RADIUS = 150.0f;
    static constexpr float SPHERE_Y_POSITION = 1.0f;
    static constexpr float SPACING_MULTIPLIER = 2.5f;
    static constexpr float MIN_COLOR_VALUE = 0.3f;   // Increased for more saturated metallic colors
    static constexpr float MAX_COLOR_VALUE = 1.0f;
    static constexpr float MIN_Z_DEPTH = -400.0f;
    static constexpr float MAX_Z_DEPTH = 400.0f;
    static constexpr float SPHERE_REFLECTIVITY = 0.0f;  // Increased for more mirror-like metallic surfaces
};
