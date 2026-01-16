# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( conversions_test_suite  conversions_catch2_main.cpp
                                        test_BoolsVisitor.cpp
                                        test_ConversionTraits.cpp
                                        test_DoublesVisitor.cpp
                                        test_IntegersVisitor.cpp
                                        test_SizetsVisitor.cpp
                                        test_StringsVisitor.cpp )

# Add required libraries as needed for conversions_test_suite
target_link_libraries( conversions_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( conversions_test_suite PRIVATE Catch2::Catch2WithMain )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directorry!
catch_discover_tests( conversions_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies ad excludes
if(BUILD_COVERAGE) 
    get_target_property(conversions_test_sources conversions_test_suite SOURCES)
    list(TRANSFORM conversions_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
    psmrts_register_code_coverage(DEPENDENCY "conversions_test_suite" EXCLUDES "${conversions_test_sources}")
endif(BUILD_COVERAGE)
