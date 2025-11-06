#ifndef ANTIALIASING_HPP
#define ANTIALIASING_HPP

#include <vector>
#include <memory>
#include "Shape.hpp"
#include "Color.hpp"
#include "Vec3.hpp"

/**
 * @class AntiAliasing
 * @brief Handles Supersampling Anti-Aliasing (SSAA) for raytracing
 *
 * Eliminates jagged edges by casting multiple rays per pixel in a regular grid.
 * Each sample is positioned at the center of its sub-pixel cell and colors are averaged.
 *
 * Performance impact:
 * - samplesPerAxis = 2 → 4 rays/pixel (2x2 grid)   - Fast, noticeable improvement
 * - samplesPerAxis = 4 → 16 rays/pixel (4x4 grid)  - High quality, recommended
 * - samplesPerAxis = 8 → 64 rays/pixel (8x8 grid)  - Ultra quality, very slow
 */
class AntiAliasing {
private:
    int samplesPerAxis_;
    int totalSamples_;
    float invSamplesPerAxis_;
    float invTotalSamples_;

public:
    /**
     * @brief Construct an AntiAliasing sampler
     * @param samplesPerAxis Number of samples along each axis (total = samplesPerAxis^2)
     */
    explicit AntiAliasing(int samplesPerAxis = 4);

    /**
     * @brief Sample a single pixel with anti-aliasing
     *
     * Casts multiple rays per pixel in a regular grid pattern and averages the results.
     *
     * @param pixelX X coordinate of the pixel
     * @param pixelY Y coordinate of the pixel
     * @param imageWidth Width of the image
     * @param imageHeight Height of the image
     * @param camOrigin Camera origin position
     * @param lowerLeftCorner Lower-left corner of the viewport
     * @param horizontal Horizontal viewport vector
     * @param vertical Vertical viewport vector
     * @param scene Vector of shapes in the scene
     * @return Anti-aliased color for the pixel
     */
    Color SamplePixel(
        int pixelX, int pixelY,
        int imageWidth, int imageHeight,
        const Vec3& camOrigin,
        const Vec3& lowerLeftCorner,
        const Vec3& horizontal,
        const Vec3& vertical,
        const std::vector<std::unique_ptr<Shape>>& scene
    ) const;

    /**
     * @brief Get the number of samples per axis
     * @return Number of samples along each axis
     */
    int GetSamplesPerAxis() const { return samplesPerAxis_; }

    /**
     * @brief Get the total number of samples per pixel
     * @return Total samples (samplesPerAxis^2)
     */
    int GetTotalSamples() const { return totalSamples_; }
};

#endif // ANTIALIASING_HPP
