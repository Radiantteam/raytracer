add_library(color
        Color.cpp
)

target_include_directories(color
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
