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
