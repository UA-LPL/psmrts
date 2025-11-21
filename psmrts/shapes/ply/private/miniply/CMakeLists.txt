# Add sources current directory to include path
target_sources(psmrts PRIVATE miniply.cpp 
                         PUBLIC
                           FILE_SET psmrts_headers 
                             TYPE HEADERS 
                             FILES miniply.h
              )

if(BUILD_COVERAGE)
  psmrts_register_code_coverage(EXCLUDES miniply.cpp miniply.h)
endif()              