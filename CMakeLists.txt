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
set( PSMRTS_TRACER_DIR   ${CMAKE_SOURCE_DIR}/tracers )
set( PSMRTS_SHAPE_DIR   ${CMAKE_SOURCE_DIR}/shapes )

set( VERSION_MAJOR 0)
set( VERSION_MINOR 1)
set( VERSION_PATCH 0)
set( VERSION_DATE  "2026-07-16")
# set( PSMRTS_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" )

# Configure the project include/header files 
configure_file( psmrts_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/src/psmrts_version.h @ONLY )
include(psmrts_includes)

# Set psmrts source and include directory
set( PSMRTS_BUILD_DIR    ${CMAKE_BINARY_DIR} )

# Set up link dependencies for PSMRTS
find_package( spdlog CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
find_package( Eigen3 CONFIG REQUIRED )
find_package( Bullet CONFIG REQUIRED )
find_library( SPICE_LIB cspice )
find_package( tinyobjloader CONFIG REQUIRED)
find_package( unofficial-miniply CONFIG REQUIRED )

add_library( psmrts-dependencies INTERFACE)
target_link_libraries( psmrts-dependencies INTERFACE nlohmann_json::nlohmann_json )
target_link_libraries( psmrts-dependencies INTERFACE Eigen3::Eigen )
target_link_libraries( psmrts-dependencies INTERFACE "${SPICE_LIB}" )
target_link_libraries( psmrts-dependencies INTERFACE ${BULLET_LIBRARIES} )
target_link_libraries( psmrts-dependencies INTERFACE tinyobjloader::tinyobjloader_double )
target_link_libraries( psmrts-dependencies INTERFACE unofficial::miniply::miniply )
target_link_libraries( psmrts-dependencies INTERFACE spdlog::spdlog )
add_library( psmrts::psmrts-dependencies ALIAS psmrts-dependencies )

# Create the PSMRTS interface export target
add_library( psmrts INTERFACE )
target_include_directories( psmrts INTERFACE "${PSMRTS_INCLUDE_DIRS}" )
target_link_libraries( psmrts INTERFACE psmrts::psmrts-dependencies )
add_library( psmrts::psmrts ALIAS psmrts )

# Build C-API here if requested
if( BUILD_CAPI_APPS )
  add_subdirectory( capi )
endif( BUILD_CAPI_APPS )

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

