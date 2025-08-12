# Add ellipsoid classes to PSRMTS target

# This currently relies on NAIF, which should already be in psrmts-dependencies
target_sources(psmrts PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/EllipsoidTracerModel.cpp)
target_include_directories(psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
# add_subdirectory( private )    # Probably don't need this

# Add test directory if BUILD_TESTS and/or BUILD_COVERAGE is set
if( ${BUILD_TESTS} )
  add_subdirectory(tests)
endif( ${BUILD_TESTS} )
