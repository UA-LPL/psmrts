# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( ellipsoid_test_suite  ellipsoid_catch2_main.cpp
                                      test_EllipsoidTracer.cpp
                                      test_EllipsoidTracerModel.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( ellipsoid_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( ellipsoid_test_suite PRIVATE Catch2::Catch2WithMain )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( ellipsoid_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(ellipsoid_test_sources ellipsoid_test_suite SOURCES)
  list(TRANSFORM ellipsoid_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_log_code_coverage(DEPENDENCY "ellipsoid_test_suite" EXCLUDES "${ellipsoid_test_sources}")
endif(BUILD_COVERAGE)
