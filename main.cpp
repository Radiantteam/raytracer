#include "include/Renderer.hpp"

int main()
{
    render_scene(1920, 1080, 500.0f, "test.png");
    std::cout << "Wrote test.png\n";
}
