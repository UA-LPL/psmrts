# Add naifdsk classes to PSRMTS target
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES  NaifDskTracer.hpp
              )
add_subdirectory( private )    # May not need this

# Add test directory if BUILD_TESTING and/or BUILD_COVERAGE is set
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
