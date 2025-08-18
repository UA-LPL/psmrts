# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

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
  get_target_property(bullet_test_sources bullet_test_suite SOURCES)
  list(TRANSFORM bullet_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
  psmrts_register_code_coverage(DEPENDENCY "bullet_test_suite" EXCLUDES "${bullet_test_sources}")
endif(BUILD_COVERAGE)
