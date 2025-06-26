# capi/apps/CMakeLists.txt

set( PSMRTS_CAPI_APPS_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources and build specifics
add_executable( psmrts_capi_features  psmrts_capi_features.cpp )
target_link_libraries( psmrts_capi_features PRIVATE psmrts::psmrts_c )
#add_subdirectory(psmrts_capi_features)
