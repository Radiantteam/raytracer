file(GLOB TEST_SOURCES "units/*.cpp")

add_executable(unit_tests
        ${TEST_SOURCES}
)

target_link_libraries(unit_tests
        PRIVATE
        raytracer_lib
)

target_include_directories(unit_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
