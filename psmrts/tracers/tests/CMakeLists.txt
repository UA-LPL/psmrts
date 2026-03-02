# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( tracers_test_suite  tracers_catch2_main.cpp
                                   test_PsmrtsTracer.cpp
                                   test_PsmrtsPriorityTracer.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( tracers_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( tracers_test_suite PRIVATE Catch2::Catch2WithMain )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( tracers_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(PSMRTS_BUILD_COVERAGE)
  get_target_property(tracers_test_sources tracers_test_suite SOURCES)
  list(TRANSFORM tracers_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "tracers_test_suite" EXCLUDES "${tracers_test_sources}")
endif(PSMRTS_BUILD_COVERAGE)

