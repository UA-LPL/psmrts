# Add sources current directory to include path
target_sources( psmrts PRIVATE PsmrtsTracer.hpp )

add_subdirectory( bullet )
add_subdirectory( ellipsoid )
add_subdirectory( naifdsk )

