#pragma once
#include <cmath>
#include <algorithm>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vec3 operator+(const Vec3& v) const { return {x+v.x, y+v.y, z+v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x-v.x, y-v.y, z-v.z}; }
    Vec3 operator*(float s)       const { return {x*s, y*s, z*s}; }
    Vec3 operator/(float s)       const { return {x/s, y/s, z/s}; }

    Vec3& operator+=(const Vec3& v){ x+=v.x; y+=v.y; z+=v.z; return *this; }

    // unaire négatif -> permet d'obtenir l'opposé d'un vecteur
    Vec3 operator-() const { return {-x, -y, -z}; }
};

inline float dot(const Vec3& a, const Vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline float length(const Vec3& v) {
    return std::sqrt(dot(v,v));
}

inline Vec3 normalize(const Vec3& v) {
    float L = length(v);
    if (L == 0.0f) return {0,0,0};
    return {v.x/L, v.y/L, v.z/L};
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    float k = 2.0f * dot(v, n);
    return {v.x - k*n.x, v.y - k*n.y, v.z - k*n.z};
}