add_library(antialiasing
    AntiAliasing.cpp
    AntiAliasing.hpp
)

target_include_directories(antialiasing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/color
    ${PROJECT_SOURCE_DIR}/src/vec
    ${PROJECT_SOURCE_DIR}/src/ray
)
