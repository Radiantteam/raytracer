#include "include/Renderer.hpp"

int main()
{
    render_scene(3840, 2180, 500.0f, "image.png");
    std::cout << "Wrote image.png\n";
}
