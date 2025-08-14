# capi/CMakeLists.txt

set( PSMRTS_CAPI_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources, headers, and build specifics
add_library( psmrts_c STATIC ${PSMRTS_CAPI_DIR}/psmrts_c.cpp
                             ${PSMRTS_CAPI_DIR}/psmrts_c.h)
target_include_directories(psmrts_c PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)
#target_include_directories( psmrts_c INTERFACE "${PSMRTS_INCLUDE_DIRS}" )
target_link_libraries( psmrts_c PRIVATE psmrts::psmrts )
add_library( psmrts::psmrts_c ALIAS psmrts_c )

# Build all C applications
if(BUILD_CAPI_APPS)
  add_subdirectory( apps )
endif(BUILD_CAPI_APPS)

if(BUILD_TESTS)
 add_subdirectory(tests)
endif(BUILD_TESTS)

# add subdirectory for tests (see other test directories)
