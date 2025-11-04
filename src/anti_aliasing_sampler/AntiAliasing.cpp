#include "AntiAliasing.h"
#include <cstdlib>
#include <ctime>

AntiAliasing::AntiAliasing(int samplesPerPixel)
    : _samplesPerPixel(samplesPerPixel)
{
}

void AntiAliasing::InitRandom()
{
    srand(static_cast<unsigned>(time(nullptr)));
}

float AntiAliasing::RandomOffset()
{
    return (rand() / float(RAND_MAX)) - 0.5f;
}

Color AntiAliasing::ComputePixelColor(
    int x, int y,
    const std::vector<std::unique_ptr<Shape>>& scene,
    const Color& fallbackColor,
    float rayOriginZ
) const
{
    float r = 0.0f, g = 0.0f, b = 0.0f;

    // Lancer plusieurs rayons par pixel avec jittering aléatoire
    for (int sample = 0; sample < _samplesPerPixel; ++sample)
    {
        // Offsets aléatoires dans le pixel
        float offsetX = RandomOffset();
        float offsetY = RandomOffset();

        // Position du rayon avec offset
        float rayX = x + offsetX;
        float rayY = y + offsetY;

        // Créer et tracer le rayon
        Ray ray(Vec3(rayX, rayY, rayOriginZ), Vec3(0, 0, -1));
        Color pixelColor = ray.TraceScene(scene);

        // Si aucune intersection, utiliser la couleur de fallback
        if (!(pixelColor.R() > 0.0f || pixelColor.G() > 0.0f || pixelColor.B() > 0.0f))
        {
            pixelColor = fallbackColor;
        }

        // Accumuler les composantes
        r += pixelColor.R();
        g += pixelColor.G();
        b += pixelColor.B();
    }

    // Moyenne des samples
    r /= _samplesPerPixel;
    g /= _samplesPerPixel;
    b /= _samplesPerPixel;

    return Color(r, g, b);
}