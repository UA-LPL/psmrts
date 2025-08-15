# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add the test directory for includes#set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Build all-in-one renderer test application
add_executable( obj_test_suite  obj_catch2_main.cpp
                                test_ObjShape.cpp
                                test_PsmrtsOBJFormat.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( obj_test_suite PRIVATE psmrts::psmrts
                                              Catch2::Catch2WithMain )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
catch_discover_tests( obj_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  psmrts_log_code_coverage(DEPENDENCY "obj_test_suite" EXCLUDES "${CMAKE_CURRENT_BINARY_DIR}/*")
endif(BUILD_COVERAGE)
