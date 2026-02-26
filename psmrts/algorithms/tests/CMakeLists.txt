# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( algorithms_test_suite  algorithms_catch2_main.cpp
                                       test_algorithms.cpp )

# Add required libraries as needed for algorithms_test_suite
target_link_libraries( algorithms_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( algorithms_test_suite PRIVATE Catch2::Catch2WithMain )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directorry!
catch_discover_tests( algorithms_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies ad excludes
if(PSMRTS_BUILD_COVERAGE) 
    get_target_property(algorithms_test_sources algorithms_test_suite SOURCES)
    list(TRANSFORM algorithms_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
    psmrts_register_code_coverage(DEPENDENCY "algorithms_test_suite" EXCLUDES "${algorithms_test_sources}")
endif(PSMRTS_BUILD_COVERAGE)
