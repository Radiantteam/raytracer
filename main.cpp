#include "include/Renderer.hpp"

int main()
{
    render_scene(1920, 1080, 500.0f, "image.png");
    std::cout << "Wrote image.png\n";
}
