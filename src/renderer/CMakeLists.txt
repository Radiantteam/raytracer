add_library(renderer 
  Renderer.cpp
)

target_link_libraries(renderer
  PUBLIC
    image
    color
    ray
    shape
    sphere
    cube
    plane
    lodepng
)

target_include_directories(renderer
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
