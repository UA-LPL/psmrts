# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Add test directory for includes
set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories(${PSMRTS_PROJECT_DIR}/psmrts/tests)

# Build all-in-one renderer test application
add_executable( capi_test_suite  capi_catch2_main.cpp
                                 test_PsmrtsCAPI_Vector3D.cpp )

# Add required libraries as needed for capi_test_suite
target_link_libraries( capi_test_suite PRIVATE psmrts::psmrts_c )
target_link_libraries( capi_test_suite PRIVATE Catch2::Catch2WithMain )

include ( CTest )
include ( Catch )

catch_discover_tests( capi_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# ----- CMocka Testing Additions TEMP ------
find_package( cmocka REQUIRED )

add_executable( capi_cmocka_test_suite  cmocka_test.c )
set_source_files_properties( cmocka_test.c PROPERTIES LANGUAGE C)
set_property(TARGET capi_cmocka_test_suite PROPERTY LINKER_LANGUAGE C)

target_link_libraries( capi_cmocka_test_suite PRIVATE psmrts::psmrts_c )
target_link_libraries( capi_cmocka_test_suite PRIVATE cmocka::cmocka )

add_test( NAME capi_cmocka_test_suite COMMAND capi_cmocka_test_suite )
