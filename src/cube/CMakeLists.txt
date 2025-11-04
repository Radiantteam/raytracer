add_library(cube 
  Cube.cpp
)

target_include_directories(cube 
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
