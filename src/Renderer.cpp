#include "../include/Renderer.hpp"
#include <iostream>

void render_scene(int width, int height, const char *outputFile)
{
  Image image(width, height, Color(0.1f, 0.1f, 0.12f));

  std::vector<std::unique_ptr<Shape>> scene;
  scene.push_back(std::make_unique<Sphere>(
      Vec3(width / 2.0f, height / 2.0f, 0), 250.0f, Color(0.85f, 0.2f, 0.2f)));
  scene.push_back(std::make_unique<Sphere>(
      Vec3(width * 2.0f / 3.0f, height * 2.0f / 3.0f, 0), 200.0f, Color(0.2f, 0.85f, 0.2f)));

  Plane sol(Vec3{0.0f, -0.5f, 0.0f}, Vec3{0.0f, 1.0f, 0.0f}, Color(0.8f, 0.7f, 0.4f));
  Plane::Draw(image, sol);

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      Ray ray(Vec3(x, y, 500.0f), Vec3(0, 0, -1));
      Color pixelColor = ray.TraceScene(scene);

      if (pixelColor.R() > 0.0f || pixelColor.G() > 0.0f || pixelColor.B() > 0.0f)
        image.SetPixel(x, y, pixelColor);
    }
  }

  image.WriteFile(outputFile);
}
