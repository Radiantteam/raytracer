add_library(ray 
  Ray.cpp
)

target_include_directories(ray
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
