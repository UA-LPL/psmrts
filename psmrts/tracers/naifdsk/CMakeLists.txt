# This requires Bullet double precision, which should already be in psrmts-dependencies

# Add naifdsk classes to PSRMTS target
target_include_directories(psmrts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory( private )    # May not need this

# Add test directory if BUILD_TESTS and/or BUILD_COVERAGE is set
if( BUILD_TESTS )
  add_subdirectory(tests)
endif( BUILD_TESTS )
