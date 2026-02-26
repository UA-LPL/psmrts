# This currently relies on NAIF, which should already be in psrmts-dependencies

# Add ellipsoid classes to PSRMTS target
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES  EllipsoidTracer.hpp 
              )
add_subdirectory( private )    # May not need this

# Add test directory if PSMRTS_BUILD_TESTS and/or PSMRTS_BUILD_COVERAGE is set
if( PSMRTS_BUILD_TESTS )
  add_subdirectory(tests)
endif( PSMRTS_BUILD_TESTS )
