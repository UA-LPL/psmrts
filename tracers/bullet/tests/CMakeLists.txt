# CMakeLists.txt

# Warning: heuristically generated, may not be correct
find_package( Catch2 CONFIG REQUIRED )

# Add test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories ( ${PSMRTS_PROJECT_DIR}/psmrts/tests )


# Build all-in-one renderer test application
add_executable( bullet_test_suite  bullet_catch2_main.cpp
                                   test_BulletSystemModel.cpp )
#                                   test_PsmrtsBulletMeshMap.cpp )  

# Add required libraries as needed fo psmrts_test_suite
target_link_libraries( bullet_test_suite PRIVATE psmrts )
target_link_libraries( bullet_test_suite PRIVATE Catch2::Catch2WithMain )
target_link_libraries( bullet_test_suite PRIVATE Eigen3::Eigen )
target_link_libraries( bullet_test_suite PRIVATE "${SPICE_LIB}" )
target_link_libraries( bullet_test_suite PRIVATE ${BULLET_LIBRARIES})

include ( CTest )
include ( Catch )

catch_discover_tests( bullet_test_suite WORKING_DIRECTORY $(CMAKE_BINARY_DIR))