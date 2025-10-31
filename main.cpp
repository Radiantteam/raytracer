#include <iostream>
#include <vector>
#include <memory>

#include "Color.hpp"
#include "src/image/Image.hpp"
#include "src/sphere/Sphere.hpp"
#include "src/cube/Cube.hpp"
#include "../Shape.hpp"
#include "src/vec/Vec3.hpp"

int main() {
    const int width = 1920;
    const int height = 1080;

    // fond sombre par défaut
    Image image(width, height, Color(0.1f, 0.1f, 0.12f));

    std::vector<std::unique_ptr<Shape>> scene;
    scene.push_back(std::make_unique<Sphere>(
        Vec3(width / 2.0f, height / 2.0f, 0), 250.0f, Color(0.85f, 0.2f, 0.2f)
    ));
    scene.push_back(std::make_unique<Sphere>(
        Vec3(width / 3.0f, height / 3.0f, 0), 120.0f, Color(0.75f, 0.8f, 0.95f)
    ));
    scene.push_back(std::make_unique<Cube>(
        Vec3(width * 2.0f / 3.0f, height * 2.0f / 3.0f, 0), 200.0f, Color(0.2f, 0.85f, 0.2f)
    ));

    // Draw all shapes in the scene
    for (const auto& shape : scene) {
        shape->Draw(image);
    }

    image.WriteFile("test.png");
    std::cout << "Wrote test.png\n";
    return 0;
}