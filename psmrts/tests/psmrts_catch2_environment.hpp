#ifndef psmrts_catch2_environment_hpp
#define psmrts_catch2_environment_hpp

#include <string>

// include the Catch framework
#include "catch.hpp"
#include <catch2/catch_version.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <spdlog/spdlog.h>

/** Root path which is directory above ./build */
inline std::string psmrts_rootpath( ) {
  return ( std::string( ".." ) );
}

/**
 * @brief Constructs a path that is OS sensitive
 * 
 * @param directory    Top level directory 
 * @param pathpart     An optional subdirectory for file
 * @return std::string The constructed path
 */
inline std::string psmrts_make_path( const std::string &directory, 
                                      const std::string &pathpart = "" ) {

  if ( pathpart.size() == 0 ) {
    return ( directory );
  }

  if ( directory.size() == 0 ) {
    return ( pathpart );
  }

  size_t dirlen = directory.size();
  std::string dpathdelim = ( directory[dirlen-1] == '/' ) ? "" : "/";
  return ( directory + dpathdelim + pathpart );
}

/** Returns directory to the main ./psmrts source directory with optional subdir/file */
inline std::string psmrts_psmrts_path( const std::string &subpart = "" ) {
  return ( psmrts_make_path( psmrts_make_path( psmrts_rootpath(), "psmrts" ), subpart ) );
}

/** Returns directory to psmrts data directory with an optional subdir/file */
inline std::string psmrts_data_path( const std::string &subpart = "" ) {
  return ( psmrts_make_path( psmrts_psmrts_path( "data" ), subpart ) );
}

/** Returns directory to the main ./tracers source directory with optional subdir/file */
inline std::string psmrts_tracers_path( const std::string &subpart = "" ) {
  return ( psmrts_make_path( psmrts_make_path( psmrts_rootpath(), "tracers" ), subpart ) );
}

#endif
