cmake_minimum_required(VERSION 4.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(raytracer main.cpp
        src/sphere/Sphere.cpp
        src/sphere/Sphere.hpp
        src/vec/Vec3.hpp
)

# Add these lines
target_include_directories(raytracer PUBLIC
        "${PROJECT_BINARY_DIR}"
        "src/color"
        "src/image"
        "src/lodepng"
)

add_subdirectory(src/color)
add_subdirectory(src/image)
add_subdirectory(src/lodepng)

target_link_libraries(raytracer PUBLIC
        color
        image
        lodepng
)