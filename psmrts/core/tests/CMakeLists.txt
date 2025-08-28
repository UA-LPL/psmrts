# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( psmrts_test_suite  psmrts_catch2_main.cpp
                                   test_ProductOption.cpp
                                   test_ProductParameter.cpp
                                   test_ProductSpecification.cpp
                                   test_PsmrtsBuffer.cpp 
                                   test_PsmrtsBufferData.cpp
                                   test_PsmrtsMeshData.cpp
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
catch_discover_tests( psmrts_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  get_target_property(psmrts_test_sources psmrts_test_suite SOURCES)
  list(TRANSFORM psmrts_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "psmrts_test_suite" EXCLUDES "${psmrts_test_sources}")

  # Some additional sources in PSMRTS C API sources
  psmrts_register_code_coverage(EXCLUDES "core/tests/psmrts_catch2_environment.hpp")  
  
endif(BUILD_COVERAGE)

