#include <cmath>
#include <algorithm>
#include <limits>
#include "Plane.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"

static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }

