# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES PsmrtsShape.hpp
              )

add_subdirectory( dsk )
add_subdirectory( obj )
add_subdirectory( ply )
add_subdirectory( mesh )

if(PSMRTS_BUILD_TESTS)
    add_subdirectory( tests )
endif(PSMRTS_BUILD_TESTS)
