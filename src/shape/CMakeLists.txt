add_library(shape 
  Shape.cpp
)

target_include_directories(shape
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
