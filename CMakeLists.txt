cmake_minimum_required(VERSION 4.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(raytracer main.cpp
        src/sphere/Sphere.cpp
        src/sphere/Sphere.hpp
        src/vec/Vec3.hpp
        src/plane/Plane.cpp
        src/plane/Plane.hpp
)


# Add these lines
target_include_directories(raytracer PUBLIC
        "${PROJECT_BINARY_DIR}"
        "src/color"
        "src/image"
        "src/lodepng"
        "src/ray"
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
