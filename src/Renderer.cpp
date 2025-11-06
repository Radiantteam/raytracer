#include <vector>
#include <memory>
#include <cmath>
#include <filesystem>
#include <random>

#include "json.hpp"
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
#include "ProgressBar.hpp"
#include "AntiAliasing.hpp"
#include "Shape.hpp"
#include "SceneLoader.hpp"
#include "DNAgenerator.hpp"

void render_scene(int width, int height, float screenZ, const char *outputFile)
{
    ProgressBar progressBar(height);
    Image image(width, height);

    std::vector<std::unique_ptr<Shape>> scene;

    std::cout << "Choisir un mode:\n";
    std::cout << "1. Générer une scène JSON\n";
    std::cout << "2. Générer un nombre de sphères random\n";
    std::cout << "> ";

    int choice;
    std::cin >> choice;

    scene.push_back(std::make_unique<Plane>(
        Vec3(0, 300.0f, 0),
        Vec3(0, -1, 0),
        0.5f));

    try
    {
        if (choice == 1)
        {
            std::string outputPath = "../scene_boules.json";

            // //génère toujours la scène ADN centrée
            //  SceneGenerator::GenerateDNAAiryCentered(
            //      outputPath,
            //      width,
            //      height,
            //      300,    // nombre de paires (600 sphères)
            //      250.0f, // rayon hélice
            //      15.0f,  // espacement vertical
            //      0.18f,  // rotation par étape
            //      true,   // ponts ADN
            //      35.0f,  // rayon sphère
            //      14.0f   // rayon pont
            //  );

            // std::cout << " Scène ADN générée → " << outputPath << "\n";

            // charge la scène générée
            auto loadedScene = SceneLoader::LoadFromFile(outputPath);
            std::cout << "Loaded " << loadedScene.shapes.size() << " shapes.\n";

            // Keep your default plane, append loaded shapes
            for (auto &s : loadedScene.shapes)
                scene.push_back(std::move(s));
        }
        else if (choice == 2) {
            int sphereCount;
            std::cout << "Combien de sphères veux-tu générer ? ";
            std::cin >> sphereCount;

            if (sphereCount <= 0)
            {
                std::cout << "Aucune sphère à générer, sortie.\n";
                return;
            }

            // Add spheres to the scene
            auto spheres = SphereGenerator::Generate(sphereCount, width, height);
            for (auto &s : spheres)
                scene.push_back(std::move(s));
        }
        else
        {
            std::cerr << "Invalid option.\n";
            return;
        }
        // Add the plane to the main scene (below the spheres)
    scene.push_back(std::make_unique<Plane>(
        Vec3(0, 200.0f, 0),
        Vec3(0, -1, 0),
        0.5f));

    //Configuration caméra (at Y = 0, same level as spheres)
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

        // Raytracing avec projection perspective uniforme
        for (int j = 0; j < height; ++j)
            {
                progressBar.update(j);

                for (int i = 0; i < width; ++i)
        {
                Color pixelColor = antiAliasing.SamplePixel(
               i, j, width, height,
               camOrigin, lowerLeftCorner, horizontal, vertical,
               scene
           );



                image.SetPixel(i, j, pixelColor);
            }
        }

    image.WriteFile("test.png");
}

    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Working directory: " << std::filesystem::current_path() << "\n";
        return;
    }
}
