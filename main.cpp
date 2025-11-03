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

int main()
{
    const int width = 3920;
    const int height = 2080;

    // fond sombre par défaut
    Image image(width, height, Color(0.1f, 0.1f, 0.12f));

    std::vector<std::unique_ptr<Shape>> scene;
    scene.push_back(std::make_unique<Sphere>(
        Vec3(width / 2.0f, height / 2.0f, 0), 250.0f, Color(0.85f, 0.2f, 0.2f)));
    scene.push_back(std::make_unique<Sphere>(
        Vec3(width * 2.0f / 3.0f, height * 2.0f / 3.0f, 0), 200.0f, Color(0.2f, 0.85f, 0.2f)));
    Plane sol(
        Vec3{0.0f, -0.5f, 0.0f}, // un point du plan (y négatif → “sol”)
        Vec3{0.0f, 1.0f, 0.0f},  // normale vers le haut
        Color(0.8f, 0.7f, 0.4f));

    Plane::Draw(image, sol);

    // Simple raytracing : un rayon par colonne, du haut vers le bas
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Ray ray(Vec3(x, y, 500.0f), Vec3(0, 0, -1));
            Color pixelColor = ray.TraceScene(scene);

            // Ne dessiner que si une forme est touchée (couleur non-noire)
            if (pixelColor.R() > 0.0f || pixelColor.G() > 0.0f || pixelColor.B() > 0.0f)
            {
                image.SetPixel(x, y, pixelColor);
            }
        }
    }

    image.WriteFile("test.png");
    std::cout << "Wrote test.png\n";
    return 0;
}
