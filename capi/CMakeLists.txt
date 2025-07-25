# capi/CMakeLists.txt

set( PSMRTS_CAPI_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the C API sources, headers, and build specifics
add_library( psmrts_c STATIC ${PSMRTS_CAPI_DIR}/psmrts_c.cpp )
set( PSMRTS_INCLUDE_DIRS  ${PSMRTS_CAPI_DIR}/
                          ${PSMRTS_INCLUDE_DIRS} )
target_include_directories( psmrts_c PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
                                             $<INSTALL_INTERFACE:${PSMRTS_INSTALL_INCLUDE_DIR}>
                                     PRIVATE "${BULLET_ROOT_DIR}/${BULLET_INCLUDE_DIR}")
target_include_directories( psmrts_c PRIVATE ${PSMRTS_INCLUDE_DIRS} )
target_link_libraries( psmrts_c PRIVATE psmrts )

install(FILES ${PSMRTS_CAPI_DIR}/psmrts_c.h DESTINATION ${PSMRTS_INSTALL_INCLUDE_DIR})

# Build all C applications
add_subdirectory( apps )


# add subdirectory for tests (see other test directories)
