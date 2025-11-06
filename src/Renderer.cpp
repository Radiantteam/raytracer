#include <vector>
#include <memory>
#include <cmath>

#include "Color.hpp"
#include "Image.hpp"
#include "Plane.hpp"
#include "Vec3.hpp"
#include "Renderer.hpp"
#include "SphereGenerator.hpp"
#include "ProgressBar.hpp"
#include "AntiAliasing.hpp"
#include "Shape.hpp"

void render_scene(int width, int height, const char *outputFile)
{
    ProgressBar progressBar(height);
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
        Vec3(0, 200.0f, 0),
        Vec3(0, -1, 0),
        0.5f));

    // Configuration caméra (at Y = 0, same level as spheres)
    Vec3 camOrigin = {width / 2.0f, 0.0f, -2500.0f};

    // Field of View - Ajuste entre 40° (peu de déformation) et 70° (vue plus large)
    float fovDegrees = 50.0f;
    float aspectRatio = (float)width / (float)height;

    // Calcul de la hauteur du plan image à distance 1.0
    float viewportHeight = 1.0f * std::tan((fovDegrees * M_PI / 180.0f) / 2.0f);
    float viewportWidth = viewportHeight * aspectRatio;

    // Vecteurs de la caméra (regardant vers +Z)
    Vec3 w = normalize(Vec3(0, 0, 1));  // Direction avant (vers la scène)
    Vec3 u = normalize(Vec3(1, 0, 0));  // Direction droite
    Vec3 v = normalize(Vec3(0, 1, 0));  // Direction haut (CORRIGÉ: positif au lieu de négatif)

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

    // Raytracing avec projection perspective uniforme + Anti-Aliasing
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
