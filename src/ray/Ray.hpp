#pragma once

#include "../vec/Vec3.hpp"

struct Ray
{
  Vec3 o; // origin
  Vec3 d; // (normalized) direction

  Ray() = default;
  Ray(const Vec3 &origin, const Vec3 &dir) : o(origin), d(dir) {}

  // point at parameter t: r(t) = o + t * d
  inline Vec3 at(float t) const { return o + d * t; }
};
