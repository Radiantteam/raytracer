#pragma once
#include <random>

#include "../Shape.hpp"
#include "../vec/Vec3.hpp"
#include "image/Image.hpp"

// Dessine une sphère centrée (cx, cy), rayon en pixels, teintée par baseColor.
// Éclairage minimal : ambiant + Lambert

enum class TextureType {
    SolidColor,
    Gradient,
    Marble,
    Noise
};

class Sphere : public Shape {
public:
    Sphere(const Vec3& center, float radius, const Color& color, float reflectivity = 0.0f);

    bool Intersect(const Vec3 &o, const Vec3 &d, float &out_t) override;

    void Draw(Image& img) const ;

    const Color& GetColor() const { return _color; }
    Color GetShadedColor(const Vec3& hitPoint) const;
    const Vec3& GetCenter() const { return _center; }
    float GetReflectivity() const { return _reflectivity; }

    void RandomizeTexture();

private:
    Vec3 _center;
    float _radius;
    Color _color;
    float _reflectivity;

    TextureType _textureType;
    float _textureSeed;
};