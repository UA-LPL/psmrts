# CMakeLists.txt

cmake_minimum_required( VERSION 3.20 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

# Add option to build tests and tools
option( BUILD_TESTING   "Add PSMRTS unit tests"     OFF  )
option( BUILD_EXTRAS    "Add PSMRTS tools/extras"   OFF  )
option( BUILD_COVERAGE  "Build with code coverage"  OFF  )

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

set( PSMRTS_PROJECT_DIR  ${CMAKE_SOURCE_DIR} )
set( PSMRTS_DIR          ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_TEST_DIR     ${CMAKE_SOURCE_DIR}/psmrts/tests )
set( PSMRTS_DATA_DIR     ${CMAKE_SOURCE_DIR}/data )
set( PSMRTS_TRACER_DIR   ${CMAKE_SOURCE_DIR}/tracers )
set( PSMRTS_FORMAT_DIR   ${CMAKE_SOURCE_DIR}/formats )

set( VERSION_MAJOR 0)
set( VERSION_MINOR 0)
set( VERSION_PATCH 5)
set( VERSION_DATE  "2024-04-26")
# set( PSMRTS_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" )

# Configure the project include files
configure_file( psmrts_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/src/psmrts_version.h @ONLY )

# Set psmrts source and include directory
set( PSMRTS_BUILD_DIR    ${CMAKE_BINARY_DIR} )
add_library(psmrts INTERFACE)
target_include_directories(psmrts INTERFACE ${PSMRTS_DIR}/src 
                                            ${PSMRTS_TRACER_DIR}/naifdsk/src
                                            ${PSMRTS_FORMAT_DIR}/obj/src)

#find_package( spdlog CONFIG REQUIRED )
find_package( Eigen3 CONFIG REQUIRED )
find_library( SPICE_LIB cspice )

# add_executable(psmrts src/psmrts.cpp)

# Optionally add unit tests of psmrts classes
if( BUILD_TESTING )
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/tests/psmrts_test_config.h.in 
                  ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/tests/psmrts_test_config.h @ONLY )

  # Now check for code coverage. Note this must be done
  # before adding the subdirectories!
  if(BUILD_COVERAGE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(
      NAME coverage
      EXECUTABLE ctest --schedule-random
      DEPENDENCIES psmrts_test_suite naif_test_suite
      EXCLUDE "build/vcpkg_installed/*"
    )
  endif(BUILD_COVERAGE)

  add_subdirectory( psmrts/tests )
  add_subdirectory( tracers/naifdsk/tests )
  add_subdirectory( formats/obj/tests )
  enable_testing()
endif( BUILD_TESTING )

