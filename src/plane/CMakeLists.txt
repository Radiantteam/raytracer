add_library(plane
    ../plane/plane.cpp
)

target_include_directories(plane
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
