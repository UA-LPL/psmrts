# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add the test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Build all-in-one renderer test application
add_executable( psmrts_test_suite  psmrts_catch2_main.cpp
                                   test_ProductParameter.cpp
                                   test_ProductSpecification.cpp
                                   test_PsmrtsBuffer.cpp 
                                   test_PsmrtsBufferData.cpp
                                   test_PsmrtsMeshData.cpp
                                   test_PsmrtsParameters.cpp
                                   test_PsmrtsPriorityTracer.cpp
                                   test_PsmrtsRayTrace.cpp
                                   test_PsmrtsRequest.cpp
                                   test_PsmrtsShapeTracer.cpp
                                   test_PsmrtsStridingBuffer.cpp
                                   test_PsmrtsTracerModel.cpp
                                   test_PsmrtsUtilities.cpp 
                                   test_PsmrtsVector3.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( psmrts_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( psmrts_test_suite PRIVATE Catch2::Catch2WithMain )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
catch_discover_tests( psmrts_test_suite )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  psmrts_log_code_coverage(DEPENDENCY "psmrts_test_suite" EXCLUDES "${CMAKE_CURRENT_BINARY_DIR}/*")
endif(BUILD_COVERAGE)

