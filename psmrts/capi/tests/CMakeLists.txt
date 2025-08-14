# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories( ${PSMRTS_PROJECT_DIR} )
include_directories( ${PSMRTS_PROJECT_DIR}/psmrts )
include_directories( ${PSMRTS_PROJECT_DIR}/psmrts/core)
include_directories( ${PSMRTS_PROJECT_DIR}/psmrts/core/tests )

# Build all-in-one renderer test application
add_executable( capi_test_suite  capi_catch2_main.cpp
                                 test_PsmrtsCAPI.cpp )

# Add required libraries as needed for capi_test_suite
target_link_libraries( capi_test_suite PRIVATE psmrts::psmrts_c )
target_link_libraries( capi_test_suite PRIVATE Catch2::Catch2WithMain )

include ( CTest )
include ( Catch )

set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
catch_discover_tests( capi_test_suite WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# ----- CMocka Testing Additions TEMP ------
find_package( cmocka REQUIRED )

add_executable( capi_cmocka_test_suite  cmocka_test.c )
set_source_files_properties( cmocka_test.c PROPERTIES LANGUAGE C)
set_property(TARGET capi_cmocka_test_suite PROPERTY LINKER_LANGUAGE C)

target_link_libraries( capi_cmocka_test_suite PRIVATE psmrts::psmrts_c )
target_link_libraries( capi_cmocka_test_suite PRIVATE cmocka::cmocka )

add_test( NAME capi_cmocka_test_suite COMMAND capi_cmocka_test_suite )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  set( PSMRTS_COVERAGE_DEPENDENCIES ${PSMRTS_COVERAGE_DEPENDENCIES} capi_test_suite  capi_cmocka_test_suite)
  set( PSMRTS_COVERAGE_EXCLUDES     ${PSMRTS_COVERAGE_EXCLUDES}     "${CMAKE_CURRENT_BINARY_DIR}/*" )
endif(BUILD_COVERAGE)
