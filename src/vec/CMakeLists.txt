add_library(vec INTERFACE)

target_include_directories(vec
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR} /..
)
