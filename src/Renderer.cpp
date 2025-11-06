#include <vector>
#include <memory>
#include <cmath>
#include <thread> // <-- 1. ADDED: Include for multithreading

#include "Color.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cube.hpp"
#include "Shape.hpp"
#include "Vec3.hpp"
#include "Renderer.hpp"
#include "SphereGenerator.hpp"

void render_scene(int width, int height, const char *outputFile)
{
    Image image(width, height);

    int sphereCount;
    std::cout << "Combien de sphères veux-tu générer ? ";
    std::cin >> sphereCount;

    if (sphereCount <= 0)
    {
        std::cout << "Aucune sphère à générer, sortie.\n";
        return;
    }

    std::vector<std::unique_ptr<Shape>> scene = SphereGenerator::Generate(sphereCount, width, height);

    // Add the plane to the main scene (below the spheres)
    scene.push_back(std::make_unique<Plane>(
        Vec3(0, 300.0f, 0),
        Vec3(0, -1, 0),
        0.5f));

    // Configuration caméra (at Y = 0, same level as spheres)
    Vec3 camOrigin = {width / 2.0f, 0.0f, -2500.0f};

    // Field of View - Ajuste entre 40° (peu de déformation) et 70° (vue plus large)
    float fovDegrees = 50.0f;
    float aspectRatio = (float)width / (float)height;

    // Calcul de la hauteur du plan image à distance 1.0
    float viewportHeight = 2.0f * std::tan((fovDegrees * M_PI / 180.0f) / 2.0f);
    float viewportWidth = viewportHeight * aspectRatio;

    // Vecteurs de la caméra (regardant vers +Z)
    Vec3 w = normalize(Vec3(0, 0, 1)); // Direction avant (vers la scène)
    Vec3 u = normalize(Vec3(1, 0, 0)); // Direction droite
    Vec3 v = normalize(Vec3(0, 1, 0)); // Direction haut (CORRIGÉ: positif au lieu de négatif)

    // Coin inférieur gauche du plan image virtuel
    Vec3 horizontal = u * viewportWidth;
    Vec3 vertical = v * viewportHeight;
    Vec3 lowerLeftCorner = camOrigin + w - horizontal * 0.5f - vertical * 0.5f;

    std::cout << "Rendu avec FOV: " << fovDegrees << "°" << std::endl;


    // ----- 2. REPLACED: Original loop is now multithreaded -----

    // Get number of available threads
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2; // Fallback
    std::cout << "Rendu avec " << numThreads << " threads." << std::endl;

    std::vector<std::thread> threads;
    int chunkHeight = height / numThreads;

    // This lambda function contains the exact logic from your original loop
    // 
    auto renderChunk = [&](int j_start, int j_end)
    {
        // Raytracing avec projection perspective uniforme
        for (int j = j_start; j < j_end; ++j) // Each thread works on a subset of 'j'
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

                // SetPixel is thread-safe here because no two threads
                // will ever write to the same 'j' row.
                image.SetPixel(i, j, pixelColor);
            }
        }
    };

    // Launch threads
    for (int t = 0; t < numThreads; ++t)
    {
        int j_start = t * chunkHeight;
        // Ensure the last thread covers all remaining rows
        int j_end = (t == numThreads - 1) ? height : (t + 1) * chunkHeight;

        threads.emplace_back(renderChunk, j_start, j_end);
    }

    // Wait for all threads to finish
    for (auto& t : threads)
    {
        t.join();
    }
    // ----- END of threading changes -----


    image.WriteFile("test.png");
}