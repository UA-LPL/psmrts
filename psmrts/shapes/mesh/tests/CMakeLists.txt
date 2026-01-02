# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one render test application
add_executable( mesh_test_suite mesh_catch2_main.cpp
                                test_MeshShape.cpp )

# Add required libraries as needed for psmrts_test_suite
target_link_libraries( mesh_test_suite PRIVATE psmrts::psmrts Catch2::Catch2WithMain )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( mesh_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(mesh_test_sources mesh_test_suite SOURCES)
  list(TRANSFORM mesh_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "mesh_test_suite" EXCLUDES "${mesh_test_sources}")
endif(BUILD_COVERAGE)
