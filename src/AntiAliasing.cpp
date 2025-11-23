/**
 * ============================================================================
 * AntiAliasing.cpp - Supersampling Anti-Aliasing (SSAA) Implementation
 * ============================================================================
 *
 * WHAT THIS FILE DOES:
 * This file implements anti-aliasing to make rendered images look smoother
 * by eliminating "jaggies" (stair-step patterns) on edges of objects. Without
 * anti-aliasing, curved objects like spheres would have pixelated, rough edges.
 * With anti-aliasing, edges appear smooth and natural.
 *
 * THE PROBLEM WE'RE SOLVING:
 * In basic ray tracing, we cast ONE ray through the center of each pixel.
 * This creates aliasing artifacts:
 * - Sharp edges look jagged and pixelated
 * - Curved surfaces (like spheres) have visible stair-stepping
 * - Small details may be missed entirely
 *
 * Think of it like taking a photo with low vs high resolution - anti-aliasing
 * gives us the "high resolution" smooth appearance.
 *
 * ALGORITHM: SUPERSAMPLING ANTI-ALIASING (SSAA)
 *
 * Instead of casting 1 ray per pixel, we cast N×N rays per pixel arranged in
 * a regular grid pattern. Then we AVERAGE all the colors to get the final
 * pixel color.
 *
 * Example with samplesPerAxis = 2 (2×2 grid = 4 rays per pixel):
 *
 *     Pixel boundary
 *     ┌─────────────┐
 *     │  •     •    │  • = sample point (ray cast here)
 *     │             │
 *     │  •     •    │  Each • is at position (i+0.5)/N where i ∈ [0,N-1]
 *     └─────────────┘
 *
 * With 2×2: Sample positions are at 0.25 and 0.75 within the pixel
 * With 4×4: Sample positions are at 0.125, 0.375, 0.625, 0.875
 *
 * HOW IT WORKS (Step by step):
 *
 * Step 1: For given pixel (x, y), create a grid of sample points
 * Step 2: For each sample point in the grid:
 *         - Calculate exact sub-pixel position (centered in grid cell)
 *         - Convert to normalized screen coordinates [0, 1]
 *         - Calculate point on virtual image plane
 *         - Create ray from camera through that point
 *         - Trace ray to get color
 *         - Accumulate color (add to running total)
 * Step 3: Average all accumulated colors (divide by total samples)
 * Step 4: Return final anti-aliased pixel color
 *
 * WHY THIS CREATES SMOOTH EDGES:
 *
 * At object boundaries, some sample rays hit the object (colored pixels) while
 * others hit the background (background color). When we average them, we get
 * an intermediate color that creates a smooth transition:
 *
 * Example at sphere edge:
 * - 2 samples hit sphere (red) → contribute red
 * - 2 samples hit background (black) → contribute black
 * - Average = 50% red + 50% black = dark red (smooth edge!)
 *
 * PERFORMANCE TRADE-OFF:
 *
 * samplesPerAxis = 2  → 4 rays per pixel   → 4× slower  → Good quality
 * samplesPerAxis = 4  → 16 rays per pixel  → 16× slower → High quality
 * samplesPerAxis = 8  → 64 rays per pixel  → 64× slower → Ultra quality
 *
 * Rendering time scales as O(samplesPerAxis²), so doubling the samples
 * quadruples the render time!
 *
 * IMPLEMENTATION DETAILS:
 *
 * - Pre-computed values: invSamplesPerAxis_ and invTotalSamples_ are computed
 *   once in the constructor to avoid repeated divisions (optimization)
 *
 * - Float accumulation: RGB values are accumulated as raw floats instead of
 *   Color objects to avoid premature clamping during addition, which would
 *   break the averaging and create artifacts
 *
 * - Centered sampling: Adding 0.5 before dividing centers each sample within
 *   its grid cell, providing better distribution than corner sampling
 *
 * - Regular grid: We use a regular (uniform) grid rather than random sampling
 *   for predictable, consistent results without noise
 */

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
