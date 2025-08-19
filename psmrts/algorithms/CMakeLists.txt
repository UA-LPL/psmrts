# Add sources current directory to include path
target_sources( psmrts PUBLIC TraningBasics.hpp )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
