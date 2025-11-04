#ifndef ANTIALIASING_HPP
#define ANTIALIASING_HPP

#include "Color.hpp"
#include "Ray.hpp"
#include "../Shape.hpp"
#include <vector>
#include <memory>

/**
 * Classe gérant l'anti-aliasing par supersampling (Random Jitter)
 */
class AntiAliasing
{
public:
    /**
     * Constructeur
     * @param samplesPerPixel Nombre de rayons par pixel (ex: 4, 9, 16)
     */
    explicit AntiAliasing(int samplesPerPixel = 4);

    /**
     * Calcule la couleur finale d'un pixel avec anti-aliasing
     * @param x Position X du pixel
     * @param y Position Y du pixel
     * @param scene Scène à tracer
     * @param fallbackColor Couleur par défaut si aucune intersection
     * @param rayOriginZ Position Z de l'origine des rayons
     * @return Couleur finale moyennée
     */
    Color ComputePixelColor(
        int x, int y,
        const std::vector<std::unique_ptr<Shape>>& scene,
        const Color& fallbackColor,
        float rayOriginZ = 500.0f
    ) const;

    /**
     * Initialise le générateur aléatoire (à appeler une fois au début)
     */
    static void InitRandom();

    // Getter
    int GetSamplesPerPixel() const { return _samplesPerPixel; }

private:
    int _samplesPerPixel;

    /**
     * Génère un décalage aléatoire entre -0.5 et +0.5
     */
    static float RandomOffset();
};

#endif // ANTIALIASING_HPP