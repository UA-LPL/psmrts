# capi/apps/CMakeLists.txt

set( PSMRTS_CAPI_APPS_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources and build specifics
add_executable( psmrts_capi_features  psmrts_capi_features.cpp )
target_link_libraries( psmrts_capi_features PRIVATE psmrts::psmrts_c )

add_executable( c_demo psmrts_capi_c_features.c)
target_include_directories(c_demo PRIVATE ${CMAKE_SOURCE_DIR}/capi)
target_link_libraries(c_demo PRIVATE psmrts::psmrts_c) 
