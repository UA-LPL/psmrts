# capi/CMakeLists.txt
include(GenerateExportHeader)
set( PSMRTS_CAPI_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources, headers, and build specifics
add_library( psmrts_c ${PSMRTS_BUILD_TYPE})
add_library( psmrts::psmrts_c ALIAS psmrts_c )
set_target_properties( psmrts_c PROPERTIES 
                                  VERSION ${PROJECT_VERSION} 
                                  SOVERSION ${PROJECT_VERSION_MAJOR} )
generate_export_header( psmrts_c )

target_sources( psmrts_c PRIVATE psmrts_c.cpp 
                         PUBLIC
                           FILE_SET psmrts_c_headers 
                             TYPE HEADERS
                             FILES psmrts_c.h
                           FILE_SET generated_headers 
                             TYPE HEADERS
                             BASE_DIRS
                               ${CMAKE_CURRENT_BINARY_DIR}
                             FILES ${CMAKE_CURRENT_BINARY_DIR}/psmrts_c_export.h
              )

target_link_libraries( psmrts_c PUBLIC psmrts::psmrts )

# Build all C applications
if(PSMRTS_BUILD_CAPI_APPS)
  add_subdirectory( apps )
endif(PSMRTS_BUILD_CAPI_APPS)

if(PSMRTS_BUILD_TESTS)
  add_subdirectory(tests)
endif(PSMRTS_BUILD_TESTS)
