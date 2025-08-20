#pragma once

#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

#include <psmrts/core/PsmrtsUtilities.hpp>


/// This section adds a JSON translation for Eigen::Quaterniond. In order
/// for some of this to work as "json j_qd = Eigen::Quaterniond();", the
/// two methods, to_json() and from_json(), must exist in the same
/// namespace the type is defined. Some namespaces may not allow that.
/// See https://json.nlohmann.me/features/arbitrary_types/#simplify-your-life-with-macros.
#define INCLUDE_EIGEN_JSON_TYPES 1
#if defined(INCLUDE_EIGEN_JSON_TYPES)
namespace Eigen {

  /** Assign an Eigen::Quaterniond to a JSON object */
  inline void to_json( json &j, const Eigen::Quaterniond &q )  {
    j = { { "w", q.w() }, { "x", q.x() }, { "y", q.y() }, { "z", q.z() } };
  }

  /** Translate a JSON object to an Eigen::Quaterniond  */
  inline void from_json( const json &j, Eigen::Quaterniond &q )  {
    q = Eigen::Quaterniond( { j["w"], j["x"], j["y"], j["z"] } );
  }    

  /** Translate an Eigen::Vector3d to JSON array */
  inline void to_json( json &j, const Eigen::Vector3d &v )  {
    j = json::array( { v[0], v[1], v[2] } );
  }

  /** Translate an Eigen::Vector3i to JSON array */
  inline void to_json( json &j, const Eigen::Vector3i &v )  {
    j = json::array( { v[0], v[1], v[2] } );
  }

  /** Translate a JSON object to an Eigen::Quaterniond  */
  inline void from_json( json &j, Eigen::Vector3d &v) {
    auto j_v = j.template get<std::vector<double>>();
    v = Eigen::Vector3d( j_v.data() );
  }    

  /** Translate a JSON object to an Eigen::Quaterniond  */
  inline void from_json( json &j, Eigen::Vector3i &v)  {
    auto j_v = j.template get<std::vector<int>>();
    v = Eigen::Vector3i( j_v.data() );
  }
  
}  // namespace Eigen
#endif


// In this psmrts namespace section below add any translations needed. This
// can occur for any type in the declaration within the namespace
namespace psmrts {

  ////  JSON I/O API
  namespace json_utils { 


    /** Declare a single constant for database  */
    inline std::string null( ) {
      return ( std::string("null") );
    }

    /** Declare a single constant for database  */
    inline json json_null( ) {
      return ( json() );
    }

    /** Declare a single constant for database  */
    inline std::string json_bool( const bool &t_or_f ) {
      if ( true == t_or_f ) return ( std::string( "true" ) );
      return ( std::string( "false" ) );
    }

    /** Load a JSON string from a string */
    inline ordered_json parse_json_string( const std::string &jsonstring,
                                            const bool ignore_comments = false ) {

      // JSON API defaults. Callers can choose to parser through comments
      const json::parser_callback_t callback = nullptr;
      const bool allow_exceptions = true;
      return ( json::parse( jsonstring, callback, allow_exceptions, ignore_comments ) );
    }

    /** Load a JSON string from a string */
    inline std::string dump_json_string( const ordered_json &j_data,
                                          const int j_indent = 2 ) {
      return ( j_data.dump( j_indent ) );
    }

    /** Load a JSON file */
    inline ordered_json read_json_file( const std::string &filename,
                                        const bool ignore_comments = false ) {

      // JSON API defaults. Callers can choose to parser through comments
      const json::parser_callback_t callback = nullptr;
      const bool allow_exceptions = true;

      json parsedjson;
      try {
        std::ifstream inputstream(filename);
        parsedjson = json::parse( inputstream, callback, allow_exceptions, ignore_comments );
      }
      catch ( const json::parse_error &e ) {
        std::string msg = std::string( e.what() ) + " in file " + filename;
        throw std::runtime_error( msg );
      }

      return ( parsedjson );
    }
    
    /**
     * @brief Creates a JSON file from ordered_json object data
     * 
     * Converts provided JSON data to a JSON file, with string name
     * identifier, and default 2nd-level indentation.
     * 
     * An indentation level of 0 will only insert newlines, -1 provides the most compact
     * representation. https://json.nlohmann.me/api/basic_json/dump/
     * 
     * @param j_data      Provided JSON data
     * @param fname       File name identifier
     * @param j_indent    Level of indentation
     */
    inline void write_json_file( const ordered_json &j_data, 
                                  const std::string &fname,
                                  const int j_indent = 2 ) {

      std::ofstream jfile ( fname );
      if ( !jfile ) {
        std::string msg = "PsmrtsParameter::write_json_file - Failed to create label file " + fname;
        throw std::runtime_error( msg );
      }

      jfile << dump_json_string( j_data, j_indent ) << std::endl;
      return;
    }
  } // namespace json_utils

} // namespace psmrts
