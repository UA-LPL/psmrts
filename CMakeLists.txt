# CMakeLists.txt

cmake_minimum_required( VERSION 3.23 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set( CMAKE_C_STANDARD 17 )

if(DEFINED VCPKG_TARGET_TRIPLET)
  message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
endif(DEFINED VCPKG_TARGET_TRIPLET)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(psmrts_register_code_coverage)

#-----------------------------------------------------------------------------
# PSMRTS build options
#-----------------------------------------------------------------------------
# Add option to build tests and tools
option( PSMRTS_BUILD_SHARED    "Build PSMRTS shared libraries"      OFF )
option( PSMRTS_BUILD_TESTS     "Add PSMRTS unit tests"              OFF )
option( PSMRTS_BUILD_COVERAGE  "Build with code coverage"           OFF )
option( PSMRTS_BUILD_EXTRAS    "Add PSMRTS tools extras"            OFF )
option( PSMRTS_BUILD_CAPI      "Build PSMRTS C API"                  ON )
option( PSMRTS_BUILD_APPS      "Build PSMRTS C++ Applications"      OFF )
option( PSMRTS_BUILD_CAPI_APPS "Build PMSRTS C Applications"        OFF )
option( PSMRTS_BUILD_DOCS      "Build API Doxygen Documentation"    OFF )
option( PSMRTS_WITH_ISIS       "Build with ISIS Shapemodel Support" OFF )

# Check for type of library build.
if(PSMRTS_BUILD_SHARED)
  set(PSMRTS_BUILD_TYPE SHARED)
else()
  set(PSMRTS_BUILD_TYPE STATIC)
endif(PSMRTS_BUILD_SHARED)

# Build PSMRTS C API.
if(PSMRTS_BUILD_APPS)
  # Ensure we build the complete C API.
  set(PSMRTS_BUILD_CAPI_APPS ON)
endif(PSMRTS_BUILD_APPS)

# Build coverage requires testing as well.
if(PSMRTS_BUILD_COVERAGE)
    set(PSMRTS_BUILD_TESTS ON)
    # Initialize configs to build up coverage targets
    psmrts_register_code_coverage(DEPENDENCY  EXCLUDES  )
endif(PSMRTS_BUILD_COVERAGE)

if(PSMRTS_BUILD_TESTS)
    enable_testing()
endif(PSMRTS_BUILD_TESTS)

# Set shared library RPATH configs here
if(PSMRTS_BUILD_SHARED)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN" "$ORIGIN/../lib")
  elseif(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path" "@loader_path/../lib" )
  endif(UNIX AND NOT APPLE)
endif(PSMRTS_BUILD_SHARED)

#-----------------------------------------------------------------------------
# PSMRTS project configuration. PROJECT_SOURCE_DIR=psmrts
#-----------------------------------------------------------------------------
project(PSMRTS VERSION 0.7.1 LANGUAGES CXX C)
set( PSMRTS_VERSION_DATE  "2026-07-14")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set( PSMRTS_PROJECT_DIR  ${CMAKE_SOURCE_DIR} )
set( PSMRTS_DIR          ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_SOURCE_DIR   ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_TEST_DIR     ${CMAKE_SOURCE_DIR}/psmrts )
set( PSMRTS_DATA_DIR     ${CMAKE_SOURCE_DIR}/core/data )
set( PSMRTS_TRACERS_DIR  ${CMAKE_SOURCE_DIR}/tracers )
set( PSMRTS_SHAPES_DIR   ${CMAKE_SOURCE_DIR}/shapes )

# Configure the project include/header files 
configure_file( psmrts_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/core/psmrts_version.h @ONLY )

# Set psmrts source and include directory
set( PSMRTS_BUILD_DIR    ${CMAKE_BINARY_DIR} )

#-----------------------------------------------------------------------------
# Set up link dependencies for PSMRTS
#-----------------------------------------------------------------------------
find_package( nlohmann_json      CONFIG REQUIRED )
find_package( Eigen3             CONFIG REQUIRED )
find_package( cspice QUIET NAMES unofficial-cspice cspice )

# Check for Conda special Bullet config
find_file(_conda_bullet_config 
             NAMES BulletConfig-float64.cmake 
             PATHS ${CMAKE_PREFIX_PATH}
             PATH_SUFFIXES lib/cmake/bullet library/lib/cmake/bullet )
if (NOT "${_conda_bullet_config}" MATCHES ".*-NOTFOUND")
  find_package( Bullet REQUIRED CONFIGS BulletConfig-float64.cmake)
else()
  # All non-conda environments assumes double is default.
  find_package( Bullet           CONFIG REQUIRED )
endif(NOT "${_conda_bullet_config}" MATCHES ".*-NOTFOUND")

find_package(Tracy  REQUIRED)

# Create standardized/common targets. Note that Bullet and cspice can be
# provided in projects by other means. This macro will check for other
# resources providing these libraries. However, they must be called before
# this macro is called. In that case, find PSMRTS after those packages
# are found. Regardless, consumers must ensure Bullet::Bullet_double and
# cspice::cspice targets exist.
include(psmrts_dependency_standards)
psmrts_create_dependency_targets()

#-----------------------------------------------------------------------------
# Set up for PSMRTS library interface.
#-----------------------------------------------------------------------------
add_library( psmrts ${PSMRTS_BUILD_TYPE} )
add_library( psmrts::psmrts ALIAS psmrts )
add_subdirectory(psmrts)
target_include_directories( psmrts PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)

# Create link depedendencies using standarized targets
#target_link_libraries( psmrts INTERFACE psmrts::psmrts-dependencies )
target_link_libraries( psmrts PUBLIC
    nlohmann_json::nlohmann_json
    Eigen3::Eigen
    ${CSPICE_LIBRARY_TARGET}
    Tracy::TracyClient
)

target_link_libraries( psmrts PRIVATE
    Bullet::Bullet_double
)

#-----------------------------------------------------------------------------
# Set up SHARED PSMRTS library versioning.
#-----------------------------------------------------------------------------
if(PSMRTS_BUILD_SHARED)
  set_target_properties( psmrts PROPERTIES VERSION ${PROJECT_VERSION} )
  # Set SOVERSION for non-WINDOWS systems.
  if(NOT WIN32 OR MINGW)
    set_target_properties( psmrts PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} )
  endif(NOT WIN32 OR MINGW)
endif(PSMRTS_BUILD_SHARED)

#-----------------------------------------------------------------------------
# Install PSMRTS targets and export/import configuration.
#-----------------------------------------------------------------------------
 install(TARGETS psmrts
         EXPORT psmrts_targets
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         FILE_SET psmrts_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/psmrts
 )

# Build C-API here if requested
if( PSMRTS_BUILD_CAPI OR PSMRTS_BUILD_CAPI_APPS )
  add_subdirectory( psmrts/capi )
  install(TARGETS psmrts_c
          EXPORT psmrts_targets
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          FILE_SET psmrts_c_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/psmrts/capi
          FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/psmrts/capi
        )
endif( PSMRTS_BUILD_CAPI OR PSMRTS_BUILD_CAPI_APPS )

# Export target configurations
install(EXPORT psmrts_targets
          FILE psmrts-targets.cmake 
          NAMESPACE psmrts:: 
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/psmrts 
        )

  
# Create and write the PSMRTS package configuration file.
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/psmrts-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/psmrts-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/psmrts
)

# Create and install version package.
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/psmrts-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Install PSMRTS configuration files.
install(FILES 
         "${CMAKE_CURRENT_BINARY_DIR}/psmrts-config.cmake"
         "${CMAKE_CURRENT_BINARY_DIR}/psmrts-config-version.cmake"
         "${CMAKE_CURRENT_SOURCE_DIR}/cmake/psmrts_dependency_standards.cmake"
        DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/psmrts )

#-----------------------------------------------------------------------------
# Optionally build unit tests and code coverage of PSMRTS classes
#-----------------------------------------------------------------------------
if( PSMRTS_BUILD_TESTS )
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/core/tests/psmrts_test_config.h.in 
                  ${CMAKE_CURRENT_SOURCE_DIR}/psmrts/core/tests/psmrts_test_config.h @ONLY )

  include(CTest)

  # Now check for code coverage. Note this must be done
  # before adding the subdirectories!
  if(PSMRTS_BUILD_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    # Must add these flags to the INTERFACE portion of PSMRTS or it won't
    # pick up the headers!
    target_compile_options(psmrts INTERFACE -g0 --coverage)
    target_link_options(psmrts    INTERFACE -g0 --coverage)


    # Exclude any instances of external files. lcov behaves
    # differently than gcovr so try to exclude as much as we
    # can.
    # Set code coverage dependencies and exclude properties 
    get_property(psmrts_cc_depends  GLOBAL PROPERTY PSMRTS_COVERAGE_DEPENDENCIES )
    get_property(psmrts_cc_excludes GLOBAL PROPERTY PSMRTS_COVERAGE_EXCLUDES )

    if(CODE_COVERAGE_VERBOSE)
      message("CC_Dependencies: ${psmrts_cc_depends}")
      message("CC_Exclusions:   ${psmrts_cc_excludes}")
      message("CC_BaseDir:      ${PSMRTS_SOURCE_DIR}")
    endif(CODE_COVERAGE_VERBOSE)

    # The results of gcovr are perhaps slightly more accurate as it provides a
    # parameter that omit closing scope braces from counts whereas lcov does
    # not. Use the target "lcov_coverage" in the CI to automatically report the
    # coverage results in text. 
    set(GCOVR_ADDITIONAL_ARGS --exclude-noncode-lines)
    setup_target_for_coverage_gcovr_html(
      NAME coverage
      EXECUTABLE ctest --schedule-random
      DEPENDENCIES   ${psmrts_cc_depends}
      BASE_DIRECTORY "${PSMRTS_SOURCE_DIR}"
      EXCLUDE        ${psmrts_cc_excludes}
    )

    # Set up for generation of coverage info using lcov. Some very specific
    # excludes are needed for lcov that differ from gcovr to get similar, but
    # not exact, coverage results.
    set(psmrts_lcov_args --no-external --quiet --ignore-errors gcov,source,graph )
    set(psmrts_genhtml_args --quiet --rc geninfo_unexecuted_blocks=1 )
    set(psmrts_lcov_excludes "/*/vcpkg_installed/*")
    setup_target_for_coverage_lcov(
      NAME lcov_coverage
      EXECUTABLE ctest --schedule-random
      DEPENDENCIES   ${psmrts_cc_depends}
      BASE_DIRECTORY "${PSMRTS_SOURCE_DIR}"
      EXCLUDE        ${psmrts_cc_excludes} ${psmrts_lcov_excludes} 
      LCOV_ARGS      ${psmrts_lcov_args} 
      GENHTML_ARGS   ${psmrts_genhtml_args}
    )
  endif(PSMRTS_BUILD_COVERAGE)
endif( PSMRTS_BUILD_TESTS )

#-----------------------------------------------------------------------------
# Build PSMRTS documentation
#-----------------------------------------------------------------------------
if(PSMRTS_BUILD_DOCS)
  find_package(Doxygen QUIET)

  if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating Doxygen API Documentation.."
        VERBATIM
    )
  else ()
    message(WARNING "ERROR: Doxygen not found, unable to generate documentation.")
  endif(DOXYGEN_FOUND)
endif(PSMRTS_BUILD_DOCS)
