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
target_link_libraries( psmrts_test_suite PRIVATE psmrts Catch2::Catch2WithMain )
target_include_directories( psmrts_test_suite PRIVATE ${PSMRTS_INCLUDE_DIRS} "${BULLET_ROOT_DIR}/${BULLET_INCLUDE_DIR}" )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( psmrts_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
