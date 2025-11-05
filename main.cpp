#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#include "Color.hpp"
#include "Ray.hpp"
#include "src/image/Image.hpp"
#include "src/sphere/Sphere.hpp"
#include "src/plane/Plane.hpp"
#include "src/cube/Cube.hpp"
#include "../Shape.hpp"
#include "src/vec/Vec3.hpp"

#include <random>

std::vector<std::unique_ptr<Shape>> GenerateSpheres(int count, int width, int height)
{
    std::vector<std::unique_ptr<Shape>> spheres;

    if (count <= 0) return spheres;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distC(0.2f, 1.0f);    // couleur RGB

    // Fixed radius for all spheres
    const float radius = 150.0f;

    // All spheres on horizontal line at vertical center
    float centerY = height / 2.0f;

    // Calculate spacing between sphere centers (diameter + some gap)
    float spacingX = radius * 2.5f;  // 2.5 = 2 radii + 0.5 gap

    // Calculate total width needed for all spheres
    float totalWidth = (count - 1) * spacingX;

    // Center the horizontal line
    float startX = (width - totalWidth) / 2.0f;

    // Random Z distribution for depth variation
    std::uniform_real_distribution<float> distZ(-400.0f, 400.0f);

    for (int i = 0; i < count; ++i)
    {
        float x = startX + i * spacingX;
        float z = distZ(gen);

        Color color(distC(gen), distC(gen), distC(gen));
        spheres.push_back(std::make_unique<Sphere>(Vec3{x, centerY, z}, radius, color, 0.9f));
    }

    return spheres;
}

int main()
{
    const int width = 3840;
    const int height = 2160;

    Image image(width, height);

    int sphereCount;
    std::cout << "Combien de sphères veux-tu générer ? ";
    std::cin >> sphereCount;

    if (sphereCount <= 0)
    {
        std::cout << "Aucune sphère à générer, sortie.\n";
        return 0;
    }

    std::vector<std::unique_ptr<Shape>> scene = GenerateSpheres(sphereCount, width, height);

    // Add the plane to the main scene
    scene.push_back(std::make_unique<Plane>(
        Vec3(0, height * 0.9f, 0),
        Vec3(0, -1, 0),
        0.5f));

    // Configuration caméra
    Vec3 camOrigin = {width / 2.0f, height / 2.0f, -1500.0f};

    // Field of View - Ajuste entre 40° (peu de déformation) et 70° (vue plus large)
    float fovDegrees = 50.0f;
    float aspectRatio = (float)width / (float)height;

    // Calcul de la hauteur du plan image à distance 1.0
    float viewportHeight = 2.0f * std::tan((fovDegrees * M_PI / 180.0f) / 2.0f);
    float viewportWidth = viewportHeight * aspectRatio;

    // Vecteurs de la caméra (regardant vers +Z)
    Vec3 w = normalize(Vec3(0, 0, 1));  // Direction avant (vers la scène)
    Vec3 u = normalize(Vec3(1, 0, 0));  // Direction droite
    Vec3 v = normalize(Vec3(0, 1, 0));  // Direction haut (CORRIGÉ: positif au lieu de négatif)

    // Coin inférieur gauche du plan image virtuel
    Vec3 horizontal = u * viewportWidth;
    Vec3 vertical = v * viewportHeight;
    Vec3 lowerLeftCorner = camOrigin + w - horizontal * 0.5f - vertical * 0.5f;

    std::cout << "Rendu avec FOV: " << fovDegrees << "°" << std::endl;

    // Raytracing avec projection perspective uniforme
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            // Coordonnées normalisées [0, 1]
            float u_coord = (float)i / (float)(width - 1);
            float v_coord = (float)j / (float)(height - 1);

            // Point sur le plan image virtuel
            Vec3 pixelPos = lowerLeftCorner + horizontal * u_coord + vertical * v_coord;

            // Direction du rayon (normalisée)
            Vec3 rayDir = normalize(pixelPos - camOrigin);

            Ray ray(camOrigin, rayDir);
            Color pixelColor = ray.TraceScene(scene);

            image.SetPixel(i, j, pixelColor);
        }
    }

    image.WriteFile("test.png");
    std::cout << "Wrote test.png\n";
    return 0;
}