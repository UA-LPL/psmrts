# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES MeshShape.hpp 
)

# Only add if requested
if(PSMRTS_BUILD_TESTS)
  add_subdirectory( tests )
endif(PSMRTS_BUILD_TESTS)
