file(GLOB TEST_SOURCES "units/*.cpp")

add_executable(unit_tests
  ${TEST_SOURCES}
)

target_include_directories(unit_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(unit_tests
  PRIVATE
    renderer
    image
    lodepng
    ray
    color
    shape
    sphere
    plane
    cube
    vec
)

add_test(NAME unit COMMAND unit_tests)
