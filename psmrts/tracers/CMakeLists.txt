# Add sources current directory to include path
#target_sources( psrmts PUBLIC PsmrtsTracer.hpp )
target_include_directories( psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
add_subdirectory( bullet )
add_subdirectory( ellipsoid )
add_subdirectory( naifdsk )
