#include "../include/AntiAliasing.hpp"

#include "Color.hpp"
#include "Ray.hpp"
#include "Shape.hpp"
#include "Vec3.hpp"

AntiAliasing::AntiAliasing(int samplesPerAxis)
    : samplesPerAxis_(samplesPerAxis)
    , totalSamples_(samplesPerAxis * samplesPerAxis)
    , invSamplesPerAxis_(1.0f / static_cast<float>(samplesPerAxis))
    , invTotalSamples_(1.0f / static_cast<float>(totalSamples_))
{
}

Color AntiAliasing::SamplePixel(
    int pixelX, int pixelY,
    int imageWidth, int imageHeight,
    const Vec3& camOrigin,
    const Vec3& lowerLeftCorner,
    const Vec3& horizontal,
    const Vec3& vertical,
    const std::vector<std::unique_ptr<Shape>>& scene
) const
{
    // Accumulate RGB values as raw floats to avoid premature clamping
    // Color class clamps on addition, which breaks averaging
    float r_accum = 0.0f;
    float g_accum = 0.0f;
    float b_accum = 0.0f;

    // ==================== SUPERSAMPLING ANTI-ALIASING (SSAA) ====================
    // Eliminates jagged edges by casting multiple rays per pixel in a regular grid
    // Each sample is positioned at the center of its sub-pixel cell
    //
    // Example for 4x4 grid: 16 rays evenly distributed across the pixel
    //   Offsets: 0.125, 0.375, 0.625, 0.875 (in each dimension)
    //
    // This smooths sphere edges by averaging colors at slightly different positions
    for (int sampleY = 0; sampleY < samplesPerAxis_; ++sampleY)
    {
        for (int sampleX = 0; sampleX < samplesPerAxis_; ++sampleX)
        {
            // Calculate sub-pixel offset within [0, 1]
            // Adding 0.5 centers each sample within its grid cell
            float offsetX = (sampleX + 0.5f) * invSamplesPerAxis_;
            float offsetY = (sampleY + 0.5f) * invSamplesPerAxis_;

            // Normalized screen coordinates [0, 1] with sub-pixel precision
            float u_coord = (static_cast<float>(pixelX) + offsetX) / static_cast<float>(imageWidth - 1);
            float v_coord = (static_cast<float>(pixelY) + offsetY) / static_cast<float>(imageHeight - 1);

            // Calculate point on virtual image plane
            Vec3 pixelPos = lowerLeftCorner + horizontal * u_coord + vertical * v_coord;

            // Ray direction from camera through this sub-pixel sample point
            Vec3 rayDir = normalize(pixelPos - camOrigin);

            // Cast ray and accumulate color components
            Ray ray(camOrigin, rayDir);
            Color sampleColor = ray.TraceScene(scene);

            // Accumulate without clamping
            r_accum += sampleColor.R();
            g_accum += sampleColor.G();
            b_accum += sampleColor.B();
        }
    }

    // Average all samples and create final anti-aliased pixel color
    // Clamping happens in Color constructor
    return Color(r_accum * invTotalSamples_,
                 g_accum * invTotalSamples_,
                 b_accum * invTotalSamples_);
}
