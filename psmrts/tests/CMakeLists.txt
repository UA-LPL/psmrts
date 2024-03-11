# CMakeLists.txt

# this is heuristically generated, and may not be correct
find_package( Catch2 CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )

# Add the test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Build all-in-one renderer test application
add_executable( psmrts_test_suite  psmrts_catch2_main.cpp
                                   test_PsmrtsDataModel.cpp 
                                   test_PsmrtsUtilities.cpp )


if( BUILD_COVERAGE )
  message( FATAL_ERROR "Code Coverage not yet implemented!" )
  # include( CodeCoverage )
  # append_coverage_compiler_flags()
  # set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage" )
  # set( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage" )
  # setup_target_for_coverage_lcov( NAME coverage EXECUTABLE  psmrts_test_suite )
endif()

# Add required libraries as needed for psmrts_test_suite
target_link_libraries( psmrts_test_suite PRIVATE psmrts )
target_link_libraries( psmrts_test_suite PRIVATE Catch2::Catch2WithMain )
target_link_libraries( psmrts_test_suite PRIVATE spdlog::spdlog_header_only )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( psmrts_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
