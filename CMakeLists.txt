# CMakeLists.txt

cmake_minimum_required( VERSION 3.20 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Add option to build tests and tools
option( BUILD_TESTING   "Add PSMRTS unit tests"     OFF  )
option( BUILD_EXTRAS    "Add PSMRTS tools/extras"   OFF  )
option( BUILD_COVERAGE  "Build with code coverage"  OFF  )
option( BUILD_CAPI_APPS "Build C API and apps"      ON )

# Ensure we have a valid toolchain
if( DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE )
  set( CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "" )
else()
  add_compile_definitions(TINYOBJLOADER_USE_DOUBLE)
endif()

# See https://github.com/Microsoft/vcpkg/issues/4497 as to why this
# is needed. Its a conflict with proj and nlohmann_json.
#set( other_flags "${other_flags} -frtti -fvisibility-inlines-hidden" )
#set( other_flags "${other_flags} -fvisibility=hidden" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags}" )

# This is PROJECT_SOURCE_DIR=psmrts
project( psmrts )
include(GNUInstallDirs)

set( PSMRTS_PROJECT_DIR  ${CMAKE_SOURCE_DIR} )
set( PSMRTS_DIR          ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_TEST_DIR     ${CMAKE_SOURCE_DIR}/psmrts/tests )
set( PSMRTS_DATA_DIR     ${CMAKE_SOURCE_DIR}/data )
set( PSMRTS_TRACERS_DIR   ${CMAKE_SOURCE_DIR}/tracers )
set( PSMRTS_SHAPES_DIR    ${CMAKE_SOURCE_DIR}/shapes )

set( VERSION_MAJOR 0)
set( VERSION_MINOR 2)
set( VERSION_PATCH 0)
set( VERSION_DATE  "2026-07-22")
# set( PSMRTS_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" )

# Configure the project include/header files 
configure_file( psmrts_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/src/psmrts_version.h @ONLY )
include(psmrts_includes)

# Set psmrts source and include directory
set( PSMRTS_BUILD_DIR    "${CMAKE_BINARY_DIR}" )
set( PSMRTS_INSTALL_INCLUDE_DIR "include/psmrts" )

# Set up link dependencies for PSMRTS
find_package( spdlog CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
find_package( Eigen3 CONFIG REQUIRED )
if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  find_package( Bullet CONFIGS REQUIRED )
else()
  find_package( Bullet CONFIGS BulletConfig-float64.cmake REQUIRED )
endif()
find_library( SPICE_LIB cspice )

add_library( psmrts-dependencies INTERFACE)
target_link_libraries( psmrts-dependencies INTERFACE nlohmann_json::nlohmann_json )
target_link_libraries( psmrts-dependencies INTERFACE Eigen3::Eigen )
target_link_libraries( psmrts-dependencies INTERFACE "${SPICE_LIB}" )
target_link_libraries( psmrts-dependencies INTERFACE ${BULLET_LIBRARIES} )
target_link_libraries( psmrts-dependencies INTERFACE spdlog::spdlog )

# Create the PSMRTS interface export target
add_library( psmrts STATIC ${PSMRTS_SHAPES_DIR}/ply/src/miniply.cpp 
                           ${PSMRTS_SHAPES_DIR}/obj/src/tiny_obj_loader.cc )
target_include_directories( psmrts PUBLIC $<BUILD_INTERFACE:${PSMRTS_INCLUDE_DIRS}>
                                          $<INSTALL_INTERFACE:include/psmrts> 
                                   PRIVATE "${BULLET_ROOT_DIR}/${BULLET_INCLUDE_DIR}" )
target_link_libraries( psmrts INTERFACE psmrts-dependencies )

set( PSMRTS_TARGETS psmrts psmrts-dependencies )
# Build C-API here if requested
if( BUILD_CAPI_APPS )
  add_subdirectory( capi )
  set( PSMRTS_TARGETS ${PSMRTS_TARGETS}
                      psmrts_c )
endif( BUILD_CAPI_APPS )

install(TARGETS psmrts LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Generate the package config
configure_file(cmake/config.cmake.in
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
              @ONLY)

# Install the package config
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
              DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Install the library
install(TARGETS ${PSMRTS_TARGETS}
        EXPORT psmrtsTargets
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${PSMRTS_INSTALL_INCLUDE_DIR})

# Install the headers
install(FILES ${PSMRTS_HEADER_FILES} DESTINATION ${PSMRTS_INSTALL_INCLUDE_DIR})

# Install the target
install(EXPORT psmrtsTargets
        NAMESPACE psmrts::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Optionally add unit tests of psmrts classes
if( BUILD_TESTING )
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/tests/psmrts_test_config.h.in 
                  ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/tests/psmrts_test_config.h @ONLY )

  # Now check for code coverage. Note this must be done
  # before adding the subdirectories!
  if(BUILD_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(
      NAME coverage
      EXECUTABLE ctest --schedule-random
      DEPENDENCIES psmrts_test_suite 
                   bullet_test_suite
                   ellipsoid_test_suite
                   naif_test_suite
                   dsk_test_suite
                   obj_test_suite
                   ply_test_suite
      BASE_DIRECTORY "${PSMRTS_PROJECT_DIR}"
      EXCLUDE "psmrts/tests/*"
              "shapes/dsk/tests/*"
              "shapes/obj/tests/*"
              "shapes/ply/tests/*"      
              "tracers/bullet/tests/*"
              "tracers/ellipsoid/tests/*"
              "tracers/naifdsk/tests/*"
              "build/vcpkg_installed/*"
    )
  endif(BUILD_COVERAGE)

  add_subdirectory( psmrts/tests )
  add_subdirectory( shapes/dsk/tests )
  add_subdirectory( shapes/obj/tests )
  add_subdirectory( shapes/ply/tests )  
  add_subdirectory( tracers/naifdsk/tests )
  add_subdirectory( tracers/ellipsoid/tests )
  add_subdirectory( tracers/bullet/tests )

  enable_testing()
endif( BUILD_TESTING )
