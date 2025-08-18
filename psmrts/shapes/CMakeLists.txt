# Add sources current directory to include path
target_sources( psmrts PRIVATE PsmrtsShape.hpp )

add_subdirectory( dsk )
add_subdirectory( obj )
add_subdirectory( ply )

