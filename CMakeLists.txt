# CMakeLists.txt

cmake_minimum_required( VERSION 3.20 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set( CMAKE_C_STANDARD 17 )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Add option to build tests and tools
option( BUILD_TESTS      "Add PSMRTS unit tests"       OFF )
option( BUILD_COVERAGE   "Build with code coverage"    OFF )
option( BUILD_EXTRAS     "Add PSMRTS tools extras"     OFF )
option( BUILD_CAPI       "Build PSMRTS C API"           ON )
option( BUILD_CAPI_APPS  "Build PMSRTS C API and APPS"  ON )

if(BUILD_COVERAGE)
    set(BUILD_TESTS ON)
    set( PSMRTS_COVERAGE_DEPENDENCIES "" )
    set( PSMRTS_COVERAGE_EXCLUDES "")  
endif(BUILD_COVERAGE)

if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

# See https://github.com/Microsoft/vcpkg/issues/4497 as to why this
# is needed. Its a conflict with proj and nlohmann_json.
#set( other_flags "${other_flags} -frtti -fvisibility-inlines-hidden" )
#set( other_flags "${other_flags} -fvisibility=hidden" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags}" )

# Must be set before project()
#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# This is PROJECT_SOURCE_DIR=psmrts
project(PSMRTS VERSION 0.20.0 LANGUAGES CXX C)
set( PSMRTS_VERSION_DATE  "2025-08-03")
include(GNUInstallDirs)

set( PSMRTS_PROJECT_DIR  ${CMAKE_SOURCE_DIR} )
set( PSMRTS_DIR          ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_TEST_DIR     ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_DATA_DIR     ${CMAKE_SOURCE_DIR}core//data )
set( PSMRTS_TRACERS_DIR   ${CMAKE_SOURCE_DIR}/tracers )
set( PSMRTS_SHAPES_DIR    ${CMAKE_SOURCE_DIR}/shapes )

# Configure the project include/header files 
configure_file( psmrts_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/core/psmrts_version.h @ONLY )
# include(psmrts_includes)

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
add_subdirectory(psmrts)
target_include_directories( psmrts INTERFACE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries( psmrts INTERFACE psmrts::psmrts-dependencies )
add_library( psmrts::psmrts ALIAS psmrts )

# Build C-API here if requested
if( BUILD_CAPI OR BUILD_CAPI_APPS )
  add_subdirectory( psmrts/capi )
endif( BUILD_CAPI OR BUILD_CAPI_APPS )

# Optionally add unit tests of psmrts classes
if( BUILD_TESTS )
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/core/tests/psmrts_test_config.h.in 
                  ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/core/tests/psmrts_test_config.h @ONLY )

  include(CTest)

  # Now check for code coverage. Note this must be done
  # before adding the subdirectories!
  if(BUILD_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    # Exclude any instances of VCPKG
    set( PSMRTS_COVERAGE_EXCLUDES "${PSMRTS_COVERAGE_EXCLUDES}" "build/vcpkg_installed/*" 
                                                                "vcpkg_installed/*")  

    setup_target_for_coverage_gcovr_html(
      NAME coverage
      EXECUTABLE ctest --schedule-random
      DEPENDENCIES   "${PSMRTS_COVERAGE_DEPENDENCIES}"
      BASE_DIRECTORY "${PSMRTS_PROJECT_DIR}"
      EXCLUDE        "${PSMRTS_COVERAGE_EXCLUDES}" 
    )
  endif(BUILD_COVERAGE)
  # enable_testing()
endif( BUILD_TESTS )

