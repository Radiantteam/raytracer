#include <iostream>
#include <vector>
#include <memory>

#include "Color.hpp"
#include "Ray.hpp"
#include "src/image/Image.hpp"
#include "src/sphere/Sphere.hpp"
#include "src/plane/Plane.hpp"
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
        Vec3(width / 2.0f, height / 2.0f, 0), 250.0f, Color(0.85f, 0.2f, 0.2f)));
    // scene.push_back(std::make_unique<Sphere>(
    //     Vec3(width / 3.0f, height / 3.0f, 0), 120.0f, Color(0.75f, 0.8f, 0.95f)));
    scene.push_back(std::make_unique<Cube>(
        Vec3(width * 2.0f / 3.0f, height * 2.0f / 3.0f, 0), 200.0f, Color(0.2f, 0.85f, 0.2f)
    ));
    Plane sol(
    Vec3{0.0f, -0.5f,  0.0f},   // un point du plan (y négatif → “sol”)
    Vec3{0.0f,  1.0f,  0.0f},   // normale vers le haut
    Color(0.8f, 0.7f, 0.4f)
    );

    Plane::Draw(image, sol);
    // camera au point d'origine, screen plane at z = 1
    Vec3 camera{0.0f, 0.0f, 0.0f};

    Vec3 center1{0.0f, 0.0f, 3.0f};
    float r1 = 1.0f;
    Color col1(0.85f, 0.2f, 0.2f);

    Vec3 center2{-1.0f, 2.0f, 4.0f};
    float r2 = 0.7f;
    Color col2(0.75f, 0.8f, 0.95f);

    const Vec3 lightDir = normalize(Vec3{0, 0, -1.0f});
    const float ambient = 0.12f;

    const float viewport_height = 1.0f;
    const float aspect = float(width) / float(height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float ndc_x = (x + 0.5f) / float(width);
            float ndc_y = (y + 0.5f) / float(height);
            float sx = (2.0f * ndc_x - 1.0f) * aspect * viewport_height;
            float sy = 1.0f - 2.0f * ndc_y; // flip Y so top = positive

            Vec3 dir = normalize(Vec3{sx, sy, 1.0f});
            Ray ray(camera, dir);

            // trace: find nearest intersection
            float t_near = 1e30f;
            bool hit = false;
            Color finalCol = image.GetPixel(x, y); // background

    // Draw all shapes in the scene
    for (const auto& shape : scene) {
        shape->Draw(image);
    }



    image.WriteFile("test.png");
    std::cout << "Wrote test.png\n";
    return 0;
}
