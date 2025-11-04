cmake_minimum_required(VERSION 4.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(raytracer main.cpp
        src/sphere/Sphere.cpp
        src/sphere/Sphere.hpp
        src/cube/Cube.cpp
        src/cube/Cube.hpp
        src/Shape.cpp
        src/vec/Vec3.hpp
        src/plane/Plane.cpp
        src/plane/Plane.hpp
        src/ray/Ray.hpp
        src/ray/Ray.cpp
        src/generator/SphereGenerator.hpp
        src/generator/SphereGenerator.cpp
        src/progress_bar/ProgressBar.cpp
        src/progress_bar/ProgressBar.h
        src/anti_aliasing/AntiAliasing.cpp
        src/anti_aliasing/AntiAliasing.h
)


# Add these lines
target_include_directories(raytracer PUBLIC
        "${PROJECT_BINARY_DIR}"
        "src"
        "src/color"
        "src/image"
        "src/lodepng"
        "src/ray"
        "src/generator"
)

add_subdirectory(src/color)
add_subdirectory(src/image)
add_subdirectory(src/lodepng)

target_link_libraries(raytracer PUBLIC
        color
        image
        lodepng
)


# --- Unit tests (doctest) ---
enable_testing()

add_executable(vec3_tests
    src/tests/units/test_vec3.cpp
        src/tests/units/test_vec3.cpp
)
# Allow `#include "src/vec/Vec3.hpp"` and `#include "doctest.h"`
target_include_directories(vec3_tests PRIVATE
    src
    src/tests
)

add_test(NAME vec3 COMMAND vec3_tests)
