# CMakeLists.txt

# Add the catch2 package
find_package( Catch2 CONFIG REQUIRED )

# Build all-in-one renderer test application
add_executable( products_test_suite  products_catch2_main.cpp
                                  test_ProductCart.cpp
                                  test_ProductConfiguration.cpp
                                  test_ProductFeature.cpp
                                  test_ProductOption.cpp
                                  test_ProductOrder.cpp
                                  test_ProductProcessing.cpp
                                  test_ProductMaker.cpp
                                  test_ProductModelDispatch.cpp
                                  test_ProductSpecification.cpp
                                  test_ProductVoidVariant.cpp )

# Add required libraries as needed for conversions_test_suite
target_link_libraries( products_test_suite PRIVATE psmrts::psmrts )
target_link_libraries( products_test_suite PRIVATE Catch2::Catch2WithMain )

# Add the tests to the system
include( CTest )
include( Catch )

# Run ctest and all catch2 tests in the build directorry!
catch_discover_tests( products_test_suite WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

# Update the test dependencies and excludes
if(PSMRTS_BUILD_COVERAGE) 
    get_target_property(products_test_sources products_test_suite SOURCES)
    list(TRANSFORM products_test_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/" )
    psmrts_register_code_coverage(DEPENDENCY "products_test_suite" EXCLUDES "${products_test_sources}")
endif(PSMRTS_BUILD_COVERAGE)
