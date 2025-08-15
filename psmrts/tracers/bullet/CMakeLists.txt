# This requires Bullet double precision, which should already be in psrmts-dependencies

# Add bullet  classes to PSRMTS target
target_sources(psmrts PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/BulletTracer.hpp)
target_include_directories(psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory( private )    # May not need this

# Add test directory if BUILD_TESTING and/or BUILD_COVERAGE is set
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
