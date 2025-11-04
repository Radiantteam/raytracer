#include "Ray.hpp"
#include "../sphere/Sphere.hpp"

Ray::Ray(const Vec3 &origin, const Vec3 &direction)
    : _origin(origin), _direction(direction)
{
}

Color Ray::TraceScene(const std::vector<std::unique_ptr<Shape>> &scene) const
{
    float closest_t = 1e30f;
    const Sphere *hit_sphere = nullptr;

    // Trouver la forme la plus proche intersectée
    for (const auto &shape : scene)
    {
        float t;
        if (shape->Intersect(_origin, _direction, t))
        {
            if (t < closest_t)
            {
                closest_t = t;
                hit_sphere = dynamic_cast<const Sphere *>(shape.get());
            }
        }
    }

    // Si intersection trouvée, calculer la couleur avec ombrage
    if (hit_sphere)
    {
        Vec3 hitPoint = PointAt(closest_t);
        return hit_sphere->GetShadedColor(hitPoint);
    }

    // Sinon retourner couleur de fond (transparente, sera ignorée)
    return Color(0.0f, 0.0f, 0.0f);
}
