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

#include <random>

std::vector<std::unique_ptr<Shape>> GenerateSpheres(int count, int width, int height)
{
    std::vector<std::unique_ptr<Shape>> spheres;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(width * 0.1f, width * 0.9f);
    std::uniform_real_distribution<float> distY(height * 0.3f, height * 0.3f);
    std::uniform_real_distribution<float> distR(40.0f, 180.0f); // rayon
    std::uniform_real_distribution<float> distC(0.2f, 1.0f);    // couleur RGB
    std::uniform_real_distribution<float> distZ(-200.0f, 200.0f); // jeu de profondeur


    for (int i = 0; i < count; ++i)
    {
        float x = distX(gen);
        float y = distY(gen);
        float radius = distR(gen);

        Color color(distC(gen), distC(gen), distC(gen));
        spheres.push_back(std::make_unique<Sphere>(Vec3{x, y, 0}, radius, color));
    }

    return spheres;
}

int main()
{
    const int width = 3920;
    const int height = 2080;

    // fond sombre par défaut
    Image image(width, height, Color(0.1f, 0.1f, 0.12f));

    int sphereCount;
    std::cout << "Combien de sphères veux-tu générer ? ";
    std::cin >> sphereCount;

    if (sphereCount <= 0)
    {
        std::cout << "Aucune sphère à générer, sortie.\n";
        return 0;
    }


        // crée les sphères
    std::vector<std::unique_ptr<Shape>> scene = GenerateSpheres(sphereCount, width, height);


    // scene.push_back(std::make_unique<Cube>(
    //     Vec3(width * 0.8f, height * 0.5f, 0),
    //     200.0f,
    //     Color(0.2f, 0.85f, 0.2f)));

    Plane sol(
        Vec3{0.0f, -0.5f, 0.0f}, // un point du plan (y négatif → “sol”)
        Vec3{0.0f, 1.0f, 0.0f},  // normale vers le haut
        Color(0.8f, 0.7f, 0.4f));
    std::vector<std::unique_ptr<Shape>> scene;
    scene.push_back(std::make_unique<Sphere>(
        Vec3(width / 2.0f, height / 2.0f, 0), 250.0f, Color(0.85f, 0.2f, 0.2f),0.9f));
    scene.push_back(std::make_unique<Sphere>(Vec3(width * 2.0f / 3.0f, height * 2.0f / 3.0f, 0), 200.0f, Color(0.9f, 0.9f, 0.9f), 0.8f));
    // Add the plane to the main scene
    scene.push_back(std::make_unique<Plane>(
        Vec3(0, height * 0.9f, 0), // A point on the plane (floor)
        Vec3(0, -1, 0),            // Normal pointing UP (towards smaller Y)
        0.5f));                    // reflectivity

    // Define a single perspective camera
    Vec3 camOrigin = {width / 2.0f, height / 2.0f, -800.0f};


    // camera au point d'origine, screen plane at z = 1
    Vec3 camera{0.0f, 0.0f, 0.0f};

    // Simple raytracing : un rayon par colonne, du haut vers le bas
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Vec3 rayTarget(x, y, 0);
            Ray ray(camOrigin, normalize(rayTarget - camOrigin));
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