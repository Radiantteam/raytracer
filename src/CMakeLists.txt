add_library(raytracer_lib STATIC
        lodepng.cpp
        Color.cpp
        Image.cpp
        Ray.cpp
        Shape.cpp
        Sphere.cpp
        Cube.cpp
        Plane.cpp
        Renderer.cpp
        SphereGenerator.cpp
        DNAgenerator.cpp
        AntiAliasing.cpp
        ProgressBar.cpp
        Timer.cpp
)

target_include_directories(raytracer_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(raytracer_lib PUBLIC cxx_std_23)
