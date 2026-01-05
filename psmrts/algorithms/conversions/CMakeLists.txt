# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers
                           TYPE HEADERS 
                           FILES ConversionTraits.hpp
                                 OptionStrings.hpp
              )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
