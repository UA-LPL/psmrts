# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( dsk_test_suite  dsk_catch2_main.cpp
                                test_DskShape.cpp
                                test_PsmrtsDSKFormat.cpp )

# Add required libraries as needed for psmrts_test_suite
target_link_libraries( dsk_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( dsk_test_suite PRIVATE Catch2::Catch2WithMain )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
catch_discover_tests( dsk_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(dsk_test_sources dsk_test_suite SOURCES)
  list(TRANSFORM dsk_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "dsk_test_suite" EXCLUDES "${dsk_test_sources}")
endif(BUILD_COVERAGE)
