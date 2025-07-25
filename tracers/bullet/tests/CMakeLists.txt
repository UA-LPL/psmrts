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
target_link_libraries( bullet_test_suite PRIVATE psmrts Catch2::Catch2WithMain )
target_include_directories( bullet_test_suite PRIVATE ${PSMRTS_INCLUDE_DIRS} "${BULLET_ROOT_DIR}/${BULLET_INCLUDE_DIR}" )

include ( CTest )
include ( Catch )

catch_discover_tests( bullet_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
