#include <iostream>
#include "src/color/Color.hpp"
#include "src/image/Image.hpp"
#include "src/vec/Vec3.hpp"
#include "src/ray/Ray.hpp"
#include "src/sphere/Sphere.hpp"

int main()
{
    const int width = 800;
    const int height = 600;
    Image image(width, height, Color(0.1f, 0.1f, 0.12f));

    // camera au point d'origine, screen plane at z = 1
    Vec3 camera{0.0f, 0.0f, 0.0f};

    Vec3 center1{0.0f, 0.0f, 3.0f};
    float r1 = 1.0f;
    Color col1(0.85f, 0.2f, 0.2f);

    Vec3 center2{-1.0f, 2.0f, 4.0f};
    float r2 = 0.7f;
    Color col2(0.75f, 0.8f, 0.95f);

    const Vec3 lightDir = normalize(Vec3{0, 0, -1.0f});
    const float ambient = 0.12f;

    const float viewport_height = 1.0f;
    const float aspect = float(width) / float(height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float ndc_x = (x + 0.5f) / float(width);
            float ndc_y = (y + 0.5f) / float(height);
            float sx = (2.0f * ndc_x - 1.0f) * aspect * viewport_height;
            float sy = 1.0f - 2.0f * ndc_y; // flip Y so top = positive

            Vec3 dir = normalize(Vec3{sx, sy, 1.0f});
            Ray ray(camera, dir);

            // trace: find nearest intersection
            float t_near = 1e30f;
            bool hit = false;
            Color finalCol = image.GetPixel(x, y); // background

            float t;
            if (Sphere::Intersect(center1, r1, ray.o, ray.d, t))
            {
                if (t < t_near)
                {
                    t_near = t;
                    hit = true;
                    Vec3 p = ray.at(t);
                    Vec3 n = normalize(p - center1);
                    float lam = std::max(0.0f, dot(n, lightDir));
                    float shade = ambient + 0.8f * lam;
                    finalCol = Color(col1.R() * shade, col1.G() * shade, col1.B() * shade);
                }
            }
            if (Sphere::Intersect(center2, r2, ray.o, ray.d, t))
            {
                if (t < t_near)
                {
                    t_near = t;
                    hit = true;
                    Vec3 p = ray.at(t);
                    Vec3 n = normalize(p - center2);
                    float lam = std::max(0.0f, dot(n, lightDir));
                    float shade = ambient + 0.8f * lam;
                    finalCol = Color(col2.R() * shade, col2.G() * shade, col2.B() * shade);
                }
            }

            image.SetPixel(x, y, finalCol);
        }
    }

    image.WriteFile("test.png");
    std::cout << "widthrote test.png\n";
    return 0;
}
