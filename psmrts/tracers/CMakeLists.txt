# Add sources current directory to include path
target_sources( psmrts PRIVATE PsmrtsTracer.hpp
                               PsmrtsPriorityTracer.hpp )

add_subdirectory( bullet )
add_subdirectory( ellipsoid )
add_subdirectory( naifdsk )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
