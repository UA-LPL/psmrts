# capi/CMakeLists.txt
include(GenerateExportHeader)
set( PSMRTS_CAPI_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources, headers, and build specifics
add_library( psmrts_c ${PSMRTS_BUILD_TYPE})
add_library( psmrts::psmrts_c ALIAS psmrts_c )
set_target_properties( psmrts_c PROPERTIES 
                                  VERSION ${PROJECT_VERSION} 
                                  SOVERSION ${PROJECT_VERSION_MAJOR} )
generate_export_header( psmrts_c
                         EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/psmrts/capi/psmrts_c_export.h )

target_sources( psmrts_c PRIVATE psmrts_c.cpp 
                         PUBLIC
                           FILE_SET psmrts_c_headers 
                             TYPE HEADERS
                             FILES psmrts_c.h
                           FILE_SET generated_headers 
                             TYPE HEADERS
                             BASE_DIRS ${CMAKE_BINARY_DIR}/psmrts/capi
                             FILES ${CMAKE_BINARY_DIR}/psmrts/capi/psmrts_c_export.h

              )
target_include_directories(psmrts_c PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries( psmrts_c PRIVATE psmrts::psmrts )

get_target_property(HeaderDirs psmrts_c HEADER_DIRS)
get_target_property(HeaderSet  psmrts_c HEADER_SET)

message(STATUS HeaderDirs=${HeaderDirs})
message(STATUS HeaderSets=${HeaderSets})

# Build all C applications
if(BUILD_CAPI_APPS)
  add_subdirectory( apps )
endif(BUILD_CAPI_APPS)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
