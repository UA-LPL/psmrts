# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( naif_test_suite  naif_catch2_main.cpp
                                 test_DskKernelModel.cpp
                                 test_DskSegment.cpp 
                                 test_KernelFileSystem.cpp
                                 test_NaifDskTracer.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( naif_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( naif_test_suite PRIVATE Catch2::Catch2WithMain )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( naif_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(PSMRTS_BUILD_COVERAGE)
  get_target_property(naif_test_sources naif_test_suite SOURCES)
  list(TRANSFORM naif_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "naif_test_suite" EXCLUDES "${naif_test_sources}")
endif(PSMRTS_BUILD_COVERAGE)
