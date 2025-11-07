#include "ShapeGenerator.hpp"
#include "Cube.hpp"
#include <random>

std::vector<std::unique_ptr<Shape>> ShapeGenerator::Generate(int count, int width, int height)
{
    std::vector<std::unique_ptr<Shape>> shapes;

    if (count <= 0)
    {
        return shapes;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distC(MIN_COLOR_VALUE, MAX_COLOR_VALUE);
    std::uniform_int_distribution<int> shapeType(0, 1); // 0 = sphere, 1 = cube
    std::uniform_real_distribution<float> distReflect(0.0f, 0.8f);

    // Calculate spacing between shape centers (diameter + some gap)
    float spacingX = SPHERE_RADIUS * SPACING_MULTIPLIER;

    // Calculate total width needed for all shapes
    float totalWidth = (count - 1) * spacingX;

    // Center the horizontal line
    float startX = (width - totalWidth) / 2.0f;

    // Random Z distribution for depth variation
    std::uniform_real_distribution<float> distZ(MIN_Z_DEPTH, MAX_Z_DEPTH);

    for (int i = 0; i < count; ++i)
    {
        float x = startX + i * spacingX;
        float z = distZ(gen);

        Color color(distC(gen), distC(gen), distC(gen));

        // Randomly decide whether to create a sphere or cube
        if (shapeType(gen) == 0)
        {
            // Create sphere
            auto sphere = std::make_unique<Sphere>(
                Vec3{x, Y_POSITION, z},
                SPHERE_RADIUS,
                color,
                distReflect(gen));
            sphere->RandomizeTexture();
            shapes.push_back(std::move(sphere));
        }
        else
        {
            // Create cube with size = diameter of sphere for consistent sizing
            float cubeSize = SPHERE_RADIUS * 2.0f;
            auto cube = std::make_unique<Cube>(
                Vec3{x, Y_POSITION, z},
                cubeSize,
                color);
            // cube->RandomizeTexture();
            shapes.push_back(std::move(cube));
        }
    }

    return shapes;
}
