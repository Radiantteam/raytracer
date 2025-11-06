#pragma once

#include "Image.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cube.hpp"
#include "Shape.hpp"
#include "Vec3.hpp"
#include <memory>
#include <vector>

void render_scene(int width, int height, float screenZ, const char *outputFile);
