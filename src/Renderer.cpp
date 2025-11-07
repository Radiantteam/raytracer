#include <vector>
#include <memory>
#include <cmath>
#include <thread>
#include <filesystem>
#include <iostream>

#include "json.hpp"
#include "Color.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "Plane.hpp"
#include "Shape.hpp"
#include "Vec3.hpp"
#include "Renderer.hpp"
#include "ShapeGenerator.hpp"
#include "AntiAliasing.hpp"
#include "SceneLoader.hpp"
#include "DNAgenerator.hpp"
#include "Timer.hpp"

void render_scene(int width, int height, float screenZ, const char *outputFile)
{
    Image image(width, height);

    std::vector<std::unique_ptr<Shape>> scene;

    std::cout << "Choisir un mode:\n";
    std::cout << "1. Générer une scène JSON\n";
    std::cout << "2. Générer un nombre de sphères random\n";
    std::cout << "> ";

    int choice;
    std::cin >> choice;

    try
    {
        if (choice == 1)
        {
            std::string outputPath = "../scene_boules.json";

            // charge la scène générée
            auto loadedScene = SceneLoader::LoadFromFile(outputPath);
            std::cout << "Loaded " << loadedScene.shapes.size() << " shapes.\n";

            // Keep your default plane, append loaded shapes
            for (auto &s : loadedScene.shapes)
            {
                scene.push_back(std::move(s));
            }

            // Add plane below JSON shapes
            scene.push_back(std::make_unique<Plane>(
                Vec3(0, 550.0f, 0), // Below the lowest shape point (Y=500)
                Vec3(0, -1, 0),
                0.5f));
        }
        else if (choice == 2)
        {
            int sphereCount;
            std::cout << "Combien de sphères veux-tu générer ? ";
            std::cin >> sphereCount;

            if (sphereCount <= 0)
            {
                std::cout << "Aucune sphère à générer, sortie.\n";
                return;
            }

            // Add shapes to the scene
            auto spheres = ShapeGenerator::Generate(sphereCount, width, height);
            for (auto &s : spheres)
                scene.push_back(std::move(s));

            // Add plane below random shapes
            scene.push_back(std::make_unique<Plane>(
                Vec3(0, 200.0f, 0), // Below the lowest shape point (Y=151)
                Vec3(0, -1, 0),
                0.5f));
        }
        else
        {
            std::cerr << "Invalid option.\n";
            return;
        }

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

        // ==================== ANTI-ALIASING CONFIGURATION ====================
        // Higher values = smoother edges but slower rendering
        // samplesPerAxis = 2 → 4 rays/pixel (2x2 grid)   - Fast, noticeable improvement
        // samplesPerAxis = 4 → 16 rays/pixel (4x4 grid)  - High quality, recommended
        // samplesPerAxis = 8 → 64 rays/pixel (8x8 grid)  - Ultra quality, very slow
        AntiAliasing antiAliasing(4);

        // ==================== Timer ====================
        Timer renderTimer;

        // ----- 2. REPLACED: Original loop is now multithreaded -----

        // Get number of available threads
        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0)
            numThreads = 2; // Fallback
        std::cout << "Rendu avec " << numThreads << " threads." << std::endl;

        std::vector<std::thread> threads;
        int chunkHeight = height / numThreads;

        auto renderChunk = [&](int j_start, int j_end)
        {
            // Raytracing avec projection perspective uniforme
            for (int j = j_start; j < j_end; ++j) // Each thread works on a subset of 'j'
            {
                for (int i = 0; i < width; ++i)
                {
                    Color pixelColor = antiAliasing.SamplePixel(
                        i, j, width, height,
                        camOrigin, lowerLeftCorner, horizontal, vertical,
                        scene);

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
        for (auto &t : threads)
        {
            t.join();
        }

        image.WriteFile(outputFile);

        renderTimer.PrintElapsed("Temps de rendu");
    }

    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Working directory: " << std::filesystem::current_path() << "\n";
    }
}
