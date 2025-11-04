add_library(sphere 
  Sphere.cpp
)

target_include_directories(sphere
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
