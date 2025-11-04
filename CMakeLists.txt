cmake_minimum_required(VERSION 4.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

add_executable(raytracer main.cpp)

target_link_libraries(raytracer
    PRIVATE
        renderer
)

target_include_directories(raytracer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- Unit tests (Doctest) ---
enable_testing()
add_subdirectory(src/tests)



# add_executable(unit_tests
#     src/tests/units/test_main.cpp
#     src/tests/units/test_vec3.cpp
#     src/tests/units/test_image.cpp
#     src/renderer/Renderer.cpp
#     src/image/Image.cpp
#     src/lodepng/lodepng.cpp
# )


# target_include_directories(unit_tests PRIVATE
#     src
#     src/tests
# )

# add_test(NAME unit COMMAND unit_tests)
