# Add current directory as the base directory for all include headers
# file sets for the subdirectories here. Base dirs are now not required
# for the subdirectory target sources 
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
              )

# Add source directories
add_subdirectory( algorithms )
add_subdirectory( core )
add_subdirectory( shapes )
add_subdirectory( tracers )

if (PSMRTS_BUILD_APPS)
  add_subdirectory( apps )
endif(PSMRTS_BUILD_APPS)