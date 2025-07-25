# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add the test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories ( ${PSMRTS_PROJECT_DIR}/psmrts/tests )

# Build all-in-one render test application
add_executable( ply_test_suite ply_catch2_main.cpp
                               test_PlyShape.cpp
                               test_PsmrtsPLYFormat.cpp )

# Add required libraries as needed for psmrts_test_suite
target_link_libraries( ply_test_suite PRIVATE psmrts Catch2::Catch2WithMain )
target_include_directories( ply_test_suite PRIVATE ${PSMRTS_INCLUDE_DIRS} )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directory!
catch_discover_tests( ply_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
