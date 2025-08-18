# This currently relies on NAIF, which should already be in psrmts-dependencies

# Add ellipsoid classes to PSRMTS target
target_sources(psmrts PRIVATE EllipsoidTracer.hpp)
add_subdirectory( private )    # May not need this

# Add test directory if BUILD_TESTING and/or BUILD_COVERAGE is set
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
