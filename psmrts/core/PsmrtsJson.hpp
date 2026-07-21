#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsJson_hpp
#define PsmrtsJson_hpp

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

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

  /** Translate a JSON object to an Eigen::Vector3d  */
  inline void from_json( json &j, Eigen::Vector3d &v) {
    auto j_v = j.template get<std::vector<double>>();
    v = Eigen::Vector3d( j_v.data() );
  }    

  /** Translate a JSON object to an Eigen::Vector3i */
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
        std::string msg = "psmrts::write_json_file - Failed to create label file " + fname;
        throw std::runtime_error( msg );
      }

      jfile << dump_json_string( j_data, j_indent ) << std::endl;
      return;
    }

    /**
     * @brief Insert/copy a JSON object it a named object
     * 
     * This function is needed to handle the often delicate operation of adding
     * an exising object to a named object. Often nlohmann JSON throws runtime
     * asserts that abort your application. This method can be used to prevent
     * this for most common needs.
     * 
     * This occurs when many PSMRTS classes return raw composite JSON that
     * become part of a larger JSON structure. They are added with as a
     * key/value pair where the argument "j" is the value. This can lead to
     * runtine assert errors particularly when adding a null JSON object.
     * 
     * For example, a ProductCart contains a ProductSpecification,
     * ProductConfiguration and a PsmrtsContainer of ProductOptions. Some of
     * these elements can be empty which results in an empty JSON object which
     * results in a runtime assert error when adding it as a named key, such as:
     * 
     * @code 
     *   object_j["key"] = json::object();
     * @endcode 
     * 
     * This method can be used as follows, as demonstrated from the ProductCart:
     * 
     * @code 
     *   ordered_json order_j = { };
     *   order_j.update( insert_object( "specification",  m_specs.to_json() )  );
     *   order_j.update( insert_object( "configuration",  m_config.to_json() )  );
     *   order_j.update( insert_object( "residualoptions",  to_json( m_residual ) ) );
     * @endcode
     * 
     * 
     * @param name  Name of the key to assign the JSON object to
     * @param j     The JSON object to add
     * @return ordered_json The constructed JSON key/value object pair
     */
    inline ordered_json insert_object( const std::string &name, 
                                       const ordered_json &j,
                                       const ordered_json &null_j = ordered_json::object() ) {
      ordered_json obj_j;
      if ( j.is_null() ) {
          obj_j[name] = null_j;
      }
      else if ( j.is_array() ) {
        obj_j[name] = j;
      }
      else {
        for ( auto &[ key, value] : j.items() ) {
          if ( value.is_null() ) {
            obj_j[name][key] = null_j;
          }
          else if ( value.is_array() ) {
            obj_j[name][key] = value;
          }
          else if ( value.is_object() ) {
            obj_j[name][key] = value;
          }
          else {
            obj_j[name][key] = value;
          }
        }
      }
      return ( obj_j );        
    }

  } // namespace json_utils

} // namespace psmrts

#endif
