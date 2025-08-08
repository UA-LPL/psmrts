#ifndef psmrts_catch2_environment_hpp
#define psmrts_catch2_environment_hpp

#include <string>

// include the Catch framework
#include "catch.hpp"
#include <catch2/catch_version.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <psmrts_version.h>
#include <psmrts_test_config.h>

#include <PsmrtsUtilities.hpp>

// Ensure this is on for all tests!
#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsBufferData.hpp>

/** Root path which is directory above ./build */
inline std::string psmrts_rootpath( ) {
  return ( std::string( PSMRTS_PROJECT_DIR ) );
}

/** Returns directory to the main ./psmrts system source directory with optional subdir/file */
inline std::string psmrts_system_path( const std::string &subpart = "" ) {
  return ( psmrts::psmrts_make_path( psmrts::psmrts_make_path( psmrts_rootpath(), "psmrts" ), subpart ) );
}

/** Returns directory to psmrts data directory with an optional subdir/file */
inline std::string psmrts_data_path( const std::string &subpart = "" ) {
  return ( psmrts::psmrts_make_path( psmrts_system_path( "data" ), subpart ) );
}

/** Returns directory to the main ./tracers source directory with optional subdir/file */
inline std::string psmrts_tracers_path( const std::string &subpart = "" ) {
  return ( psmrts::psmrts_make_path( psmrts::psmrts_make_path( psmrts_rootpath(), "tracers" ), subpart ) );
}

/** Returns directory to the main ./shapes source directory with optional subdir/file */
inline std::string psmrts_shapes_path( const std::string &subpart = "" ) {
  return ( psmrts::psmrts_make_path( psmrts::psmrts_make_path( psmrts_rootpath(), "shapes" ), subpart ) );
}

#endif
