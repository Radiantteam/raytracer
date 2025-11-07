#include "SphereGenerator.hpp"
#include <random>

std::vector<std::unique_ptr<Shape>> SphereGenerator::Generate(int count, int width, int height) {
    std::vector<std::unique_ptr<Shape>> spheres;

    if (count <= 0) {
        return spheres;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distC(MIN_COLOR_VALUE, MAX_COLOR_VALUE);
    std::uniform_real_distribution<float> distZ(MIN_Z_DEPTH, MAX_Z_DEPTH);
    std::uniform_real_distribution<float> distReflect(0.0f, 0.8f);

    // Calculate spacing between sphere centers (diameter + some gap)
    float spacingX = SPHERE_RADIUS * SPACING_MULTIPLIER;

    // Calculate total width needed for all spheres
    float totalWidth = (count - 1) * spacingX;

    // Center the horizontal line
    float startX = (width - totalWidth) / 2.0f;

    for (int i = 0; i < count; ++i) {
        float x = startX + i * spacingX;
        float z = distZ(gen);

        Color color(distC(gen), distC(gen), distC(gen));
        
        auto sphere = std::make_unique<Sphere>(
            Vec3{x, SPHERE_Y_POSITION, z},
            SPHERE_RADIUS,
            color,
            distReflect(gen)
        );
        sphere->RandomizeTexture();
        spheres.push_back(std::move(sphere));
    }

    return spheres;
}
