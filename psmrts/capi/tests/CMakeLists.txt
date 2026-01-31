# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( capi_test_suite  capi_catch2_main.cpp
                                 test_PsmrtsCAPI.cpp )

# Add required libraries as needed for capi_test_suite. Must also include
# psmrts::psmrts explicity due to use of the C++ API utilities in CAPI tests.
target_link_libraries( capi_test_suite PRIVATE psmrts::psmrts_c psmrts::psmrts )
target_link_libraries( capi_test_suite PRIVATE Catch2::Catch2WithMain )

include ( CTest )
include ( Catch )

catch_discover_tests( capi_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# ----- CMocka Testing Additions TEMP ------
find_package( cmocka REQUIRED )

add_executable( capi_cmocka_test_suite  cmocka_test.c )
set_source_files_properties( cmocka_test.c PROPERTIES LANGUAGE C)
set_property(TARGET capi_cmocka_test_suite PROPERTY LINKER_LANGUAGE C)

target_link_libraries( capi_cmocka_test_suite PRIVATE psmrts::psmrts_c )
target_link_libraries( capi_cmocka_test_suite PRIVATE cmocka::cmocka )

add_test( NAME capi_cmocka_test_suite 
               COMMAND capi_cmocka_test_suite
               WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(capi_test_sources   capi_test_suite SOURCES)
  get_target_property(cmocka_test_sources capi_cmocka_test_suite SOURCES)

  list(TRANSFORM capi_test_sources   PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  list(TRANSFORM cmocka_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )

  psmrts_register_code_coverage(DEPENDENCY "capi_test_suite" 
                                           EXCLUDES "${capi_test_sources}")
  psmrts_register_code_coverage(DEPENDENCY "capi_cmocka_test_suite"
                                           EXCLUDES "${cmocka_test_sources}")

  # Some additional sources in PSMRTS C API sources
  psmrts_register_code_coverage(EXCLUDES "capi/apps/psmrts_capi_features.cpp")
  psmrts_register_code_coverage(EXCLUDES "capi/apps/psmrts_capi_c_features.c")

endif(BUILD_COVERAGE)
