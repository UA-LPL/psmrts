# CMakeLists.txt

# this is heuristically generated, and may not be correct
find_package( Catch2 CONFIG REQUIRED )
find_package(tinyobjloader CONFIG REQUIRED)
# find_package( spdlog CONFIG REQUIRED )

# Add the test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Build all-in-one renderer test application
add_executable( psmrts_test_suite  psmrts_catch2_main.cpp
                                   test_PsmrtsBuffer.cpp 
                                   test_PsmrtsBufferData.cpp
                                   test_PsmrtsMeshData.cpp
                                   test_PsmrtsParameters.cpp
                                   test_PsmrtsPriorityTracer.cpp
                                   test_PsmrtsRayTrace.cpp
                                   test_PsmrtsRequest.cpp
                                   test_PsmrtsShapeTracerAdapter.cpp
                                   test_PsmrtsStridingBuffer.cpp
                                   test_PsmrtsTracerModel.cpp
                                   test_PsmrtsUtilities.cpp 
                                   test_PsmrtsVector3.cpp )


# Add required libraries as needed for psmrts_test_suite
target_link_libraries( psmrts_test_suite PRIVATE psmrts )
target_link_libraries( psmrts_test_suite PRIVATE Catch2::Catch2WithMain )
target_link_libraries( psmrts_test_suite PRIVATE nlohmann_json::nlohmann_json )
target_link_libraries( psmrts_test_suite PRIVATE Eigen3::Eigen)
target_link_libraries( psmrts_test_suite PRIVATE "${SPICE_LIB}" )
target_link_libraries( psmrts_test_suite PRIVATE ${BULLET_LIBRARIES})
target_link_libraries( psmrts_test_suite PRIVATE tinyobjloader::tinyobjloader_double )
# target_link_libraries( psmrts_test_suite PRIVATE spdlog::spdlog_header_only )

#  Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( psmrts_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
