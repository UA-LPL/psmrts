# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

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
catch_discover_tests( obj_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(obj_test_sources obj_test_suite SOURCES)
  list(TRANSFORM obj_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "obj_test_suite" EXCLUDES "${obj_test_sources}")
endif(BUILD_COVERAGE)
