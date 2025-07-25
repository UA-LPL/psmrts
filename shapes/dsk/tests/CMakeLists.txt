# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add the test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories( ${PSMRTS_PROJECT_DIR}/psmrts/tests )

# Build all-in-one renderer test application
add_executable( dsk_test_suite  dsk_catch2_main.cpp
                                test_DskShape.cpp
                                test_PsmrtsDSKFormat.cpp )

# Add required libraries as needed for psmrts_test_suite
target_link_libraries( dsk_test_suite PRIVATE psmrts Catch2::Catch2WithMain )
target_include_directories( dsk_test_suite PRIVATE ${PSMRTS_INCLUDE_DIRS} )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( dsk_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
