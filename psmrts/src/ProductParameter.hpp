#pragma once
#ifndef ProductParameter_hpp
#define ProductParameter_hpp

#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <exception>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsParameters.hpp>

namespace psmrts { 


  /**
   * @brief Process/maintain product parameters
   * 
   * This class maintains a JSON structure of a PSMRTS product parameter.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-29 Kris J. Becker - Original Version
   */
  class ProductParameter {
    public:

      ProductParameter( ) : m_spec_j() { }

      ProductParameter( const std::string &key, const std::string &value,
                        const ordered_json &options = json_utils::json_null() ) { 
        m_spec_j = options;
        m_spec_j[key] = value;
      }

      ProductParameter( const std::string &key, const double &value,
                        const ordered_json &options = json_utils::json_null() ) { 
        m_spec_j = options;
        m_spec_j[key] = value;
      }      

      ProductParameter( const std::string &key, const int &value,
                        const ordered_json &options = json_utils::json_null() ) { 
        m_spec_j = options;
        m_spec_j[key] = value;
      }


      /** Create parameter from contents of a JSON object */
      ProductParameter( const ordered_json &specs ) {
        m_spec_j = specs;
      }

      virtual ~ProductParameter() = default;

      /// Special static creation methods...
      static inline ProductParameter from_pvl( const char *pvl_string )  {
       // Parse a PVL string of the form "keyword=value\nkeyword=value"
       return ( ProductParameter::from_pvl( std::string( pvl_string ) ) );
      }

      static inline ProductParameter from_pvl( const std::string &s ) {
       // Parse a string of the form "keyword=value\nkeyword=value"...
          ....      
      }   
     
      /** Returns number of objects/keys in the specs */
      inline int size() const {
        return ( this->specs().size() );
      }

      /** Returns boolean confirmation if target key is in Parameters */
      inline bool contains( const std::string &key ) const {
        return ( this->specs().contains( psmrts_tolower( key ) ) );
      }

      template <class T>
        inline T value( const std::string &key, const T &v_default = T{} ) const {
          std::string key_t = psmrts_tolower( key );
          if ( this->specs().contains(  key_t ) ) {
            return ( this->specs().at( key_t ).template get<T>() );
          }

          // Not found - error!
          return ( v_default );
        }

      template <class T>
        inline void add_key( const std::string &key, const T &value ) {
          std::string key_t = psmrts_tolower( key );
          m_spec_j[key_t] = value;
        }

      inline std::string name() const {
        std::string p_name("");
        return ( this->value( "name", p_name ) );
      }

      inline std::string type() const {
        std::string p_type("");
        return ( this->value( "type", p_type ) );
      }      

      inline std::string description() const {
        std::string p_descr("");
        return ( this->value( "description", p_descr ) );
      } 
      
      inline std::string status() const {
        std::string p_status("required");
        return ( this->value( "status", p_status ) );
      }      

      inline std::vector<std::string> keywords() const {
        std::vector<std::string> p_keys{};
        for ( auto &[key, value] : this->specs().items() ) {
          p_keys.push_back( key );
        }
        return ( p_keys );
      } 

      inline std::vector<std::string> aliases() const {
        std::vector<std::string> p_alias{};
        return ( this->value( "aliases", p_alias ) );
      }   
      
      inline std::vector<std::string> file_suffixes() const {
        std::vector<std::string> p_suffixes{};
        return ( this->value( "file_suffixes", p_suffixes ) );
      }   

      /** Return the JSON structure */
      inline const ordered_json &specs( ) const {
        return ( m_spec_j );
      }

      inline bool is_required() const {
        return ( this->status() == "required" );
      }

      inline bool isa_alias( const std::string &a_key ) const {

      }      

      inline ordered_json difference( const ordered_json &other ) const {
        return ( ordered_json::diff( this->specs(), other ) );
      }     

      inline bool validate( const ProductParameter &other, const bool throwException = false ) const {

        bool it_matches = false;
         
        // This should check/compare each expected value for consistency...



        // Return match status...
        return ( it_matches );
      }

    protected:
      ordered_json  m_spec_j;

      inline bool validate_file( const std::string &fname ) const {

      }

      inline bool validate_alias( const std::string &alias ) const {

      }

  };


} // namespace psmrts

#endif