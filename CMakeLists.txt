# CMakeLists.txt

cmake_minimum_required( VERSION 3.20 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Add option to build tests and tools
option( PSMRTS_BUILD_TESTING    "Add PSMRTS unit tests"       OFF )
option( PSMRTS_BUILD_EXTRAS     "Add PSMRTS tools/extras"     OFF )
option( PSMRTS_BUILD_COVERAGE   "Build with code coverage"    OFF )
option( PSMRTS_BUILD_CAPI       "Build PSMRTS C API"           ON )
option( PSMRTS_BUILD_CAPI_APPS  "Build PMSRTS C API and APPS"  ON )

# Ensure we have a valid toolchain for vcpkg builds
if( DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE )
  set( CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "" )
endif()

# See https://github.com/Microsoft/vcpkg/issues/4497 as to why this
# is needed. Its a conflict with proj and nlohmann_json.
#set( other_flags "${other_flags} -frtti -fvisibility-inlines-hidden" )
#set( other_flags "${other_flags} -fvisibility=hidden" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags}" )

# Must be set before project()
#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)


# This is PROJECT_SOURCE_DIR=psmrts
project(psmrts VERSION 0.20.0 LANGUAGES CXX C)
set( PSMRTS_VERSION_DATE  "2025-08-03")
include(GNUInstallDirs)

set( PSMRTS_PROJECT_DIR  ${CMAKE_SOURCE_DIR} )
set( PSMRTS_DIR          ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_TEST_DIR     ${CMAKE_SOURCE_DIR}/psmrts/tests )
set( PSMRTS_DATA_DIR     ${CMAKE_SOURCE_DIR}/data )
set( PSMRTS_TRACERS_DIR   ${CMAKE_SOURCE_DIR}/tracers )
set( PSMRTS_SHAPES_DIR    ${CMAKE_SOURCE_DIR}/shapes )

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

# Create interface library that collects PSMRTS dependencies
add_library( psmrts-dependencies INTERFACE)
target_link_libraries( psmrts-dependencies INTERFACE 
    nlohmann_json::nlohmann_json
    Eigen3::Eigen
    "${SPICE_LIB}"
    ${BULLET_LIBRARIES}
    tinyobjloader::tinyobjloader_double
    unofficial::miniply::miniply
    spdlog::spdlog
)
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
                   capi_test_suite
                   capi_cmocka_test_suite 
      BASE_DIRECTORY "${PSMRTS_PROJECT_DIR}"
      EXCLUDE "psmrts/tests/*"
              "shapes/dsk/tests/*"
              "shapes/obj/tests/*"
              "shapes/ply/tests/*"      
              "tracers/bullet/tests/*"
              "tracers/ellipsoid/tests/*"
              "tracers/naifdsk/tests/*"
              "capi/tests/*"
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
  add_subdirectory( capi/tests )

  enable_testing()
endif( BUILD_TESTING )

