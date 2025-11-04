#pragma once

#include "image/Image.hpp"
#include "ray/Ray.hpp"
#include "sphere/Sphere.hpp"
#include "plane/Plane.hpp"
#include "cube/Cube.hpp"
#include "shape/Shape.hpp"
#include "vec/Vec3.hpp"
#include <memory>
#include <vector>

void render_scene(int width, int height, const char *outputFile);
