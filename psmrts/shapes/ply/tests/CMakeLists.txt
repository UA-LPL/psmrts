# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one render test application
add_executable( ply_test_suite ply_catch2_main.cpp
                               test_PlyShape.cpp
                               test_PsmrtsPLYFormat.cpp )

# Add required libraries as needed for psmrts_test_suite
target_link_libraries( ply_test_suite PRIVATE psmrts::psmrts Catch2::Catch2WithMain )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( ply_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(ply_test_sources ply_test_suite SOURCES)
  list(TRANSFORM ply_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "ply_test_suite" EXCLUDES "${ply_test_sources}")
endif(BUILD_COVERAGE)
