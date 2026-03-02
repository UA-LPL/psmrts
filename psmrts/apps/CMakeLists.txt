# apps/CMakeLists.txt

set( PSMRTS_CPP_APPS_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources and build specifics
add_executable( psmrts_product_specs  psmrts_product_specs.cpp )
target_link_libraries( psmrts_product_specs PRIVATE psmrts::psmrts )
install(TARGETS psmrts_product_specs EXPORT psmrts RUNTIME)

# Exclude PSMRTS main from code coverage
psmrts_register_code_coverage(EXCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/psmrts_product_specs.cpp")


