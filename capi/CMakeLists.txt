# capi/CMakeLists.txt

set( PSMRTS_CAPI_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources, headers, and build specifics
add_library( psmrts_c STATIC ${PSMRTS_CAPI_DIR}/psmrts_c.cpp
                             ${PSMRTS_CAPI_DIR}/psmrts_c.h)
target_include_directories(psmrts_c PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> )
target_include_directories( psmrts_c INTERFACE "${PSMRTS_INCLUDE_DIRS}" )
#target_link_libraries( psmrts_c PRIVATE psmrts::psmrts )
target_link_libraries( psmrts_c PUBLIC psmrts::psmrts )
add_library( psmrts::psmrts_c ALIAS psmrts_c )

# Build all C applications
add_subdirectory( apps )


# add subdirectory for tests (see other test directories)
