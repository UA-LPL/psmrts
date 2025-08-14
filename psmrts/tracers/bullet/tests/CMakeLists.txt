# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories ( ${PSMRTS_PROJECT_DIR}/psmrts/tests )


# Build all-in-one renderer test application
add_executable( bullet_test_suite  bullet_catch2_main.cpp
                                   test_BulletTracer.cpp
                                   test_BulletSystemModel.cpp
                                   test_BulletTracerModel.cpp
                                   test_PsmrtsBulletMeshMap.cpp
                                   test_PsmrtsBulletWorldModel.cpp )  

# Add required libraries as needed fo psmrts_test_suite
target_link_libraries( bullet_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( bullet_test_suite PRIVATE Catch2::Catch2WithMain )
include ( CTest )
include ( Catch )

catch_discover_tests( bullet_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Update the test dependencies and excludes
if(BUILD_COVERAGE)
  set( PSMRTS_COVERAGE_DEPENDENCIES ${PSMRTS_COVERAGE_DEPENDENCIES} bullet_test_suite )
  set( PSMRTS_COVERAGE_EXCLUDES     ${PSMRTS_COVERAGE_EXCLUDES}    "${CMAKE_CURRENT_BINARY_DIR}/*" )
endif(BUILD_COVERAGE)
