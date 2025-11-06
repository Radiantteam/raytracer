#pragma once

#include <algorithm>

/**
 * Math utility functions for the raytracer
 */
namespace MathUtils {
    /**
     * Clamps a floating point value to the range [0, 1]
     * @param v The value to clamp
     * @return The clamped value in [0, 1]
     */
    inline float clamp01(float v) {
        return std::max(0.0f, std::min(1.0f, v));
    }
}
