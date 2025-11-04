add_library(image 
  Image.cpp
)

target_include_directories(image
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
