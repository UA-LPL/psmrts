# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add the test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories( ${PSMRTS_PROJECT_DIR}/psmrts/tests )

# Build all-in-one renderer test application
add_executable( naif_test_suite  naif_catch2_main.cpp
                                 test_DskKernelModel.cpp 
                                 test_KernelFileSystem.cpp
                                 test_NaifDskTracer.cpp
                                 test_NaifDskTracerModel.cpp
                                 test_NaifEllipsoidShape.cpp 
                                 test_NaifPsmrtsPriorityTracer.cpp
                                 test_NaifShapeTracerAdapter.cpp
                                 test_NaifUtilities.cpp 
                                 test_PsmrtsPriorityTracer.cpp 
                                 test_PsmrtsRayTrace.cpp
                                 test_PsmrtsShapeTracerAdapter.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( naif_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( naif_test_suite PRIVATE Catch2::Catch2WithMain )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( naif_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  psmrts_log_code_coverage(DEPENDENCY "naif_test_suite" EXCLUDES "${CMAKE_CURRENT_BINARY_DIR}/*")
endif(BUILD_COVERAGE)
