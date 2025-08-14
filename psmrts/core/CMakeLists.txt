# Add sources current directory to include path
target_sources( psmrts PUBLIC PsmrtsProduct.hpp )
target_include_directories( psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

# Only add if requested
if(BUILD_TESTS)
  add_subdirectory( tests )
endif(BUILD_TESTS)
