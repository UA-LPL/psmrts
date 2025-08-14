# Add sources current directory to include path
target_include_directories( psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
add_subdirectory( dsk )
add_subdirectory( obj )
add_subdirectory( ply )

