#include <iostream>
#include "Color.hpp"
#include "src/image/Image.hpp"
#include "src/sphere/Sphere.hpp"
#include "src/plane/Plane.hpp"

using namespace std;

int main() {

    // fond sombre par défaut
    Image image(1920, 1080, Color(0.1f, 0.1f, 0.12f));

    Plane sol(
        Vec3{0.0f, -0.5f,  0.0f},   // un point du plan (y négatif → “sol”)
        Vec3{0.0f,  1.0f,  0.0f},   // normale vers le haut
        Color(0.8f, 0.7f, 0.4f)
    );

    Plane::Draw(image, sol);
    Sphere::Draw(
        image,
        image.GetWidth() / 2,
        image.GetHeight() / 2,
        250.0f,
        Color(0.85f, 0.2f, 0.2f)
    );

    Sphere::Draw(
        image,
        image.GetWidth() / 3,
        image.GetHeight() / 3,
        120.0f,
        Color(0.75f, 0.8f, 0.95f)
    );



    image.WriteFile("test.png");
    std::cout << "Wrote test.png\n";
    return 0;
}