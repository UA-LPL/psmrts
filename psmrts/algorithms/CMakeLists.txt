# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers
                           TYPE HEADERS 
                           FILES TracingBasics.hpp
                                 VariantTraits.hpp
              )

add_subdirectory( conversions )

# Only add if requested
if(PSMRTS_BUILD_TESTS)
  add_subdirectory( tests )
endif(PSMRTS_BUILD_TESTS)
