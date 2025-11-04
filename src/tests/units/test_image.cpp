#include "../doctest.h"
#include <chrono>
#include "image/Image.hpp"
#include "color/Color.hpp"
#include "lodepng/lodepng.h"
#include "renderer/Renderer.hpp"

TEST_CASE("Full image render time benchmark")
{
    const int width = 3920;
    const int height = 2080;

    auto start = std::chrono::high_resolution_clock::now();
    render_scene(width, height, "benchmark_test.png");
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;

    INFO("Render took " << duration.count() << " seconds");
    CHECK(duration.count() < 10.0); // for example, expect render < 10s
}

TEST_CASE("Image has background color, width and height")
{
    const int width = 1920;
    const int height = 1080;

    Image image(width, height, Color(0.1f, 0.1f, 0.12f));

    CHECK(image.GetWidth() == 1920);
    CHECK(image.GetHeight() == 1080);

    Color pixel = image.GetPixel(0, 0);
    CHECK(pixel.R() == 0.1f);
    CHECK(pixel.G() == 0.1f);
    CHECK(pixel.B() == 0.12f);
}
