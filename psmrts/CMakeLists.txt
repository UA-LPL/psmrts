# Add current directorty to include path
target_include_directories( psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

# Add test directory if BUILD_TESTS and/or BUILD_COVERAGE is set
add_subdirectory( core )
add_subdirectory( shapes )
add_subdirectory( tracers )
