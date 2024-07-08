#ifndef PsmrtsParameters_hpp
#define PsmrtsParameters_hpp

#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

// Different versions of the JSON library are not ABI compatible.
// When you enable JSON diagnostics, it will also cause similar
// issues because it adds debugging code to the classes at compile
// time. This results in different incompatible objects in other
// code files. Every file that uses JSON data must include this
// file first, and not allow any other direct include of 
// nlohmann/json.hpp or bad things could happen.
#ifndef JSON_DIAGNOSTICS
#define JSON_DIAGNOSTICS 1
#endif
#include <nlohmann/json.hpp>
using namespace nlohmann::literals;
using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

#include <Eigen/Geometry>
#include <PsmrtsUtilities.hpp>


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

  inline void to_json( json &j, const Eigen::Vector3d &v )  {
    j = json::array( { v[0], v[1], v[2] } );
  }

  
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
    
    inline void write_json_file( const ordered_json &j_data, 
                                  const std::string &fname,
                                  const int j_indent = 2 ) {

      std::ofstream jfile ( fname );
      if ( !jfile ) {
        std::string msg = "PsmrtsParameters::write_json_file - Failed to create label file " + fname;
        throw std::runtime_error( msg );
      }

      jfile << dump_json_string( j_data, j_indent ) << std::endl;
      return;
    }
  }
  /**
   * @brief Manage arbitrary data in JSON objects
   * 
   * The JSON keys are required to be lower case. This is enforced in the
   * get/add methods. 
   *
   * @author 2024-07-04 Kris J. Becker, UA Original Version
   */
  class PsmrtsParameters {
    public:
      PsmrtsParameters() : m_json() { }
      PsmrtsParameters( const std::string &name ) : m_json( { {"name", name } } )  { }
      virtual ~PsmrtsParameters() { }

      inline int size() const {
        return ( m_json.size() );
      }

      inline bool contains( const std::string &key ) const {
        return ( m_json.contains( psmrts_tolower( key ) ) );
      }

      inline std::string get_string_parameter( const std::string &key, 
                                               const std::string &value_d = "" ) const {
        std::string s_t = value_d;
        if ( this->contains( key ) ) {
          s_t = m_json[psmrts_tolower(key)].get<std::string>();
        }

        return ( s_t );
      }

      inline double get_double_parameter( const std::string &key, 
                                               const double &value_d = std::nan("null") ) const {
        double d_t = value_d;
        if ( this->contains( key ) ) {
          d_t = m_json[psmrts_tolower(key)].get<double>();
        }

        return ( d_t );
      }

      inline int get_int_parameter( const std::string &key, 
                                            const int &value_d = 0 ) const {
        int d_t = value_d;
        if ( this->contains( key ) ) {
          d_t = m_json[psmrts_tolower(key)].get<int>();
        }

        return ( d_t );
      }

      template <class T>
        T get_parameter( const std::string &key ) const {
          std::string key_t = psmrts_tolower( key );
          if ( m_json.contains(  key_t ) ) {
            return ( m_json.at( key_t ).template get<T>() );
          }

          // Not found - error!
          return ( json() );
        }

      template <class T>
        T get_parameter( const std::string &key, const T &value_def ) const {
          std::string key_t = psmrts_tolower( key );
          if ( m_json.contains(  key_t ) ) {
            return ( m_json.at( key_t ).get<T>() );
          }

          // Not found - return the default
          return ( value_def );
        }

      template <class T>
        void add_parameter( const std::string &key, const T &value ) {
          std::string key_t = psmrts_tolower( key );
          m_json[key_t] = value;
          return;
        }

        /** Get config parameters while optionally inserting into a named object */
        inline std::string config( const std::string &objname = "",
                                   const int indent = -1 ) const {
          if ( !objname.empty() ) { 
            ordered_json jobj;
            jobj[objname] =  m_json;
            return ( json_utils::dump_json_string( jobj, indent ) );
          }

          // Otherwise return just the parameter object
          return ( json_utils::dump_json_string( m_json, indent ) );
        }

    private:
      ordered_json m_json;
  };      

} // namespace psmrts
#endif
