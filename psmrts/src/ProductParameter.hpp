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
      // Empty default, no name handling?
      ProductParameter( ) : m_spec_j() { }
      
      ProductParameter( const std::string &name, 
                        const ordered_json &options = json_utils::json_null() ) {
        m_spec_j = options;
        m_spec_j["name"] = name;
        validate_defaults();
      }

      ProductParameter( const std::string &key, const std::string &value,
                        const ordered_json &options = json_utils::json_null() ) { 
        m_spec_j = options;
        m_spec_j[key] = value;
        validate_defaults();
      }

      ProductParameter( const std::string &key, const double &value,
                        const ordered_json &options = json_utils::json_null() ) { 
        m_spec_j = options;
        m_spec_j[key] = value;
        validate_defaults();
      }      

      ProductParameter( const std::string &key, const int &value,
                        const ordered_json &options = json_utils::json_null() ) { 
        m_spec_j = options;
        m_spec_j[key] = value;
        validate_defaults();
      }

      /** Create parameter from contents of a JSON object */
      ProductParameter( const ordered_json &specs ) {
        m_spec_j = specs;
        validate_defaults();
      }

      virtual ~ProductParameter() = default;

    
      /// Special static creation methods...
      static inline ProductParameter from_pvl( const char *pvl_string )  {
       // Parse a PVL string of the form "keyword=value\nkeyword=value"
       return ( ProductParameter::from_pvl( std::string( pvl_string ) ) );
      }
      
      /**
       * @brief Returns a ProductParameter object based on a pvl format input string
       * 
       * This function expects PVL string key/values to be separated by '=', where the 
       * first part understood to be the key, and second to be the associated value.
       * Each key/value must end, and be separated by, a ';'. 
       * - Keys are automatically converted to lowercase.
       * 
       * @param s                   String in pvl format outlined above
       * @return ProductParameter   A ProductParameter with the PVL string specifications
       */
      static inline ProductParameter from_pvl( const std::string &s ) {
        ordered_json s_specs;

        size_t start = 0;
        size_t length = 0;
        while (start < s.length()) {
          size_t equals = s.find('=', start);
          size_t semicolon = s.find(';', equals);
          if (equals == std::string::npos) {
            break;
          }

          std::string key = s.substr(start, equals - start);
          if (semicolon == std::string::npos) {
            length = std::string::npos;
          }
          else {
            length = semicolon - equals - 1; 
          }
          std::string value = s.substr(equals + 1, length);

          s_specs[psmrts::psmrts_tolower(key)] = value;

          if (semicolon == std::string::npos) {
            start = s.length();
          } else {
            start = semicolon + 1;
          }
        }

        return ProductParameter( s_specs );
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
          if (key_t == "name" && m_spec_j.size() > 0) { // to add name to empty default
            throw std::invalid_argument("Error: 'name' key is reserved and cannot be modified after object creation.");
          }
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
        for (const auto &alias_k : this->aliases()) {
            if (alias_k == a_key) {
                return true;
            }
        }
        return false;
      }      
      
      inline ordered_json difference( const ordered_json &other ) const {
        return ( ordered_json::diff( this->specs(), other ) );
      }     

      /**
       * @brief comparatively validates other ProductParameters
       * 
       * Note: Files are checked for compatability via their file extension, and if 
       * file-based should be the value of a "value" parameter. Other parameters are
       * considered valid only if they match name (or associable alias), type, and 
       * status values. 
       * 
       * e.g - if one parameter has the key/value: name: "obj_file", with the associable
       * alias key/values that includes: aliases: ["file", "obj_mesh", "mesh_file"], then 
       * any other parameter using one of those aliases instead of "name" would be considered
       * valid so long as type and status is also a match.
       * 
       * @param other      ProductParameter to be checked for validity
       * @param Exception  Flag to throw exceptions, if desired
       * @return true      ProductParameter is valid
       * @return false     ProductParameter is not valid 
       */
      inline bool validate( const ProductParameter &other, const bool throwException = false ) const {
        // This should check/compare each expected value for consistency...
        // Note: Should they strictly have the same keys, or just comparing the 
        // keys in the original?
        //  Check name, if failed - check aliases.
        if (this->name() != other.name() ) {
          if ( !validate_alias( other ) )  {
            return false;
          }
        }

        if (this->type() != other.type()) {
          return false;
        }
       
        if (this->status() != other.status()) {
          return false;
        }

        if (other.type() == "file") {
          return validate_file(other);
        }
        
        if (other.type() == "string") {
          return validate_string(other); //case sensitive? if (case_insense) - convert
        }

        // Return match status...
        return true;
      }

    protected:
      ordered_json  m_spec_j;

      inline bool validate_file( const ProductParameter &other ) const {
        if (other.contains(other.name())) { //name-value becomes key that contains file ref
          std::vector<std::string> valid_wordlist{};
          valid_wordlist = this->file_suffixes();

          std::string fext = psmrts_file_extension( other.value(other.name(), std::string("")) );

          for (const auto &fvals : valid_wordlist) {
            if (fext == fvals) {
              return true;
            }
          }
          return false; 
        }
        return false;
      }

      
      inline bool validate_alias( const ProductParameter &other ) const {
        std::vector<std::string> alias_list{};
        alias_list = this->aliases();
        for (const auto &val : alias_list) {
          if (other.name() == val) {
            return true;
          }
        }
        return false;
      }
      
      inline bool validate_string( const ProductParameter &other ) const {
        if (other.contains(other.name())) {
          std::vector<std::string> valid_wordlist{};
          valid_wordlist = this->value(other.name(), valid_wordlist);

          std::string target = psmrts_tolower(other.value(other.name(), std::string("")));

          for (const auto &vals : valid_wordlist) {
            if (target == psmrts_tolower(vals)) {
              return true;
            }
          }
        }
        return false;
      }

      inline void validate_defaults() {
        if ( !m_spec_j.contains("name") ) {
          throw std::invalid_argument("Error: Parameter requires a 'name' key with specifying value.");
        }
        if ( !m_spec_j.contains("type") ) {
          m_spec_j["type"] = "string";
        }
        if ( !m_spec_j.contains("status") ) {
          m_spec_j["status"] = "required";
        }
        if (!m_spec_j.contains("description")) {
          m_spec_j["description"] = "parameter";
        }
      }

  };


} // namespace psmrts

#endif