/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductFeature_hpp
#define ProductFeature_hpp

#include <string>
#include <vector>

#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
#define NOMINMAX
#include <windows.h>
#endif

#include <algorithm>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/AllOptionConversions.hpp>

namespace psmrts { 


  /**
   * @brief Process/maintain product features and options
   * 
   * This class maintains a JSON structure of a PSMRTS product feature.
   * A product feature is defined as key/valued pair that provides
   * input/output information, such as file names, and options that
   * alter product behavior.
   * 
   * This class object holds one JSON type key and value pair of the
   * form: "key": value. "keys" will be lowercase. value can be of any
   * valid JSON type as supported in ProductOption.
   * 
   * @author Kris J. Becker, University of Arizona
   *         Kyle A. Becker, University of Arizona
   * @history 2025-05-29 Kris J. Becker - Original Version
   */
  class ProductFeature {
    public:
      using FeatureOption     = ProductOption;
      using FeatureOptionList = PsmrtsContainer<FeatureOption>;
      using FeatureList       = FeatureOptionList::Container;
      

      ProductFeature( ) : m_options( "feature", { } ) { }
      explicit ProductFeature( const std::string &name  ) : 
                               m_options(create( name, { } ) )  { }
      explicit ProductFeature( const char *name  ) : 
                               m_options(create( name, { } ) ) { }
      explicit ProductFeature( const std::string &name, 
                               const std::initializer_list<FeatureOption> &options ) :
                              m_options( create( name,  options ) ) { }     
      explicit ProductFeature( const std::string &name, 
                               const std::vector<FeatureOption> &options ) :
                               m_options( create( name, options ) ) { }
      explicit ProductFeature( const std::string &name, 
                               const ordered_json &options ) : 
                               m_options( from_json( options, name ) ) {  }
      explicit ProductFeature( const ordered_json &specs ) :
                               m_options( from_json( specs ) ) {  }

      virtual ~ProductFeature() = default;

    
      /// Special static creation methods...
      static inline ProductFeature from_pvl( const char *pvl_string )  {
       // Parse a PVL string of the form "keyword=value\nkeyword=value"
       return ( ProductFeature::from_pvl( std::string( pvl_string ) ) );
      }
      
      /**
       * @brief Returns a ProductFeature object based on a pvl format input string
       * 
       * This function expects PVL string key/values to be separated by '=', where the 
       * first part understood to be the key, and second to be the associated value.
       * Each key/value must end, and be separated by, a ';' or a '\n'. 
       * - Keys are automatically converted to lowercase.
       * 
       * @param s                   String in pvl format outlined above
       * @return ProductFeature   A ProductFeature with the PVL string specifications
       */
      static inline ProductFeature from_pvl( const std::string &s ) {
        std::vector<FeatureOption> f_specs;
        std::string name_t( "" );

        size_t start = 0;
        size_t length = 0;
        while (start < s.length()) {
          size_t equals = s.find('=', start);
          size_t semicolon = s.find(';', equals);
          size_t newline = s.find('\n', equals);

          if (equals == std::string::npos) {
            break;
          }

          size_t delimiter = std::min(semicolon, newline);
          if (semicolon == std::string::npos) {
            delimiter = newline;
          }

          if (newline == std::string::npos) {
            delimiter = semicolon;
          }

          std::string key = s.substr(start, equals - start);
          if (delimiter == std::string::npos) {
            length = std::string::npos;
          }
          else {
            length = delimiter - equals - 1; 
          }
          std::string value = s.substr(equals + 1, length);

          std::string key_t = psmrts::psmrts_tolower(key);
          if ( "name" == key_t ) name_t = value;

          // Append the key to the list
          f_specs.push_back( FeatureOption( psmrts::psmrts_tolower(key), value ) );

          if (delimiter == std::string::npos) {
            start = s.length();
          } else {
            start = delimiter + 1;
          }
        }

        // Check for an empty structure
        if ( f_specs.size() == 0 ) {
          // Gotta throw here
          throw std::runtime_error("*** PsmrtsFeature::from_pvl() - no features found!" );
        }

        return ( ProductFeature( name_t, { f_specs } ) );
      }   
      

      /** Returns number of objects/keys in the specs */
      inline size_t size() const {
        return ( m_options.size() );
      }

      /** Returns boolean confirmation if target key is in features */
      inline bool contains( const std::string &key ) const {
        return ( m_options.contains( key ) );
      }
      
      /** Returns feature option */
      inline FeatureOption find( const std::string &key ) const {
        return ( m_options.find( key ) );
      }


      template <class T>
        inline T value( const std::string &key, const T &v_default = T{} ) const {
          std::string key_t = psmrts_tolower( key );
          if ( m_options.contains( key_t ) ) {
            ordered_json value_j = m_options.find( key_t ).to_json();
            return ( value_j.at( key_t ).template get<T>() );
          }

          // Not found - error!
          return ( v_default );
        }

      template <class T>
        inline void add_key( const std::string &key, const T &value ) {
          std::string key_t = psmrts_tolower( key );
          m_options.add( FeatureOption( key_t, value ) );
        }

      inline bool add( const FeatureOption &option ) {
        return ( m_options.add( option ) );
      }

      inline bool replace( const FeatureOption &option ) {
        return ( m_options.replace( option ) );
      }      

      inline std::string name() const {
        return ( m_options.name() );
      }

      inline std::string type() const {
        std::string p_type("string");
        return ( this->value( "type", p_type ) );
      }      

      inline std::string description() const {
        std::string p_descr("feature");
        return ( this->value( "description", p_descr ) );
      } 
      
      inline std::string status() const {
        std::string p_status("required");
        return ( this->value( "status", p_status ) );
      }      

      inline std::vector<std::string> keywords() const {
        return ( m_options.keys() );
      } 

      inline std::vector<std::string> aliases() const {
        std::vector<std::string> p_alias{};
        if ( this->contains( "aliases" ) ) {
          p_alias = OptionStringsExtractor( m_options.find( "aliases" ) ).get_all();
        }
        return ( p_alias );
      }

      inline std::vector<std::string> exclusions( ) const {
        std::vector<std::string> conflicts_with{};
        if ( this->contains( "exclusions" ) ) {
          conflicts_with = OptionStringsExtractor( m_options.find( "exclusions" ) ).get_all();
        }        
        return ( conflicts_with );
      }        
      
      inline std::vector<std::string> inclusions( ) const {
        std::vector<std::string> required_for{};
        if ( this->contains( "inclusions" ) ) {
          required_for = OptionStringsExtractor( m_options.find( "inclusions" ) ).get_all();
        }         
        return (  required_for );
      }     

      inline std::vector<std::string> file_suffixes() const {
        std::vector<std::string> p_suffixes{};
        if ( this->contains( "file_suffixes" ) ) {
          p_suffixes = OptionStringsExtractor( m_options.find( "file_suffixes" ) ).get_all();
        }        
        return ( p_suffixes );
      }   

      inline std::vector<std::string> valid_list() const {
        std::vector<std::string> p_valid{};
        if ( this->contains( "valid" ) ) {
          p_valid = OptionStringsExtractor( m_options.find( "valid" ) ).get_all();
        }        
        return ( p_valid );
      } 
      
      
      /** Return the JSON structure */
      inline ordered_json specs( ) const {
        return ( this->to_json() );
      }

      /** Checks required status for this feature */
      inline bool is_required() const {
        return ( this->status() == "required" );
      }

      /** Checks if this feature is a dependency */
      inline bool is_dependency() const {
        return ( this->status() == "dependency" );
      }

      /** Checks if this feature is optional */
      inline bool is_optional() const {
        return ( this->status() == "optional" );
      }

      /** Checks if this feature is an alias */
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

      // operator == function, call difference and if .empty(), then successful
      inline bool operator==( const ProductFeature &other ) const {
        return ( difference( other.specs() ).empty() );
      }

      /**
       * @brief comparatively validates other ProductFeatures
       * 
       * Note: Files are checked for compatability via their file extension, and if 
       * file-based should be the value of a "value" feature. Other features are
       * considered valid only if they match name (or associable alias), type, and 
       * status values. 
       * 
       * e.g - if one feature has the key/value: name: "obj_file", with the associable
       * alias key/values that includes: aliases: ["file", "obj_mesh", "mesh_file"], then 
       * any other features using one of those aliases instead of "name" would be considered
       * valid so long as type and status is also a match.
       * 
       * @param other      ProductFeature to be checked for validity
       * @param Exception  Flag to throw exceptions, if desired
       * @return true      ProductFeature is valid
       * @return false     ProductFeature is not valid 
       */
      inline bool validate( const ProductFeature &other, const bool throwException = false ) const {
        //  Check name, if failed - check aliases.
        if (this->name() != other.name() ) {
          if ( !validate_alias( other ) )  {
            return false;
          }
        }

        // If type exists..
        if ( other.contains( "type" ) ) {
          if (this->type() != other.type()) {
            return false;
          }
        }
       
        // If status exists..
        if ( other.contains( "status" ) ) {
          if (this->status() != other.status()) {
            return false;
          }
        }

        if ( this->type() == "file" ) {
          return validate_file(other);
        }
        
        if (this->type() == "string") {
          return validate_string(other); //case sensitive? if (case_insense) - convert
        }

        return true;
      }

      inline ordered_json to_json() const {
        ordered_json json_t;
        json_t["name"] =  m_options.name();

        for ( const FeatureOption &feature : m_options.data() ) {
          json_t.merge_patch( feature.to_json() );
        }
        return ( json_t );
      }

      inline const FeatureList &features() const {
        return ( m_options.data() );
      }

      /** Confirm the filename has the appropriate suffix for this feature */
      inline bool validate_file_suffix( const std::string &filename ) const {
        std::vector<std::string> sfx_v = this->file_suffixes();
        if ( sfx_v.size() == 0 ) return ( true );

        std::string suffix_f = psmrts_file_extension( filename );
        for ( const std::string &sfx : sfx_v ) {
          if ( sfx == suffix_f ) return ( true );
        }
        return ( false );
      }


    private:
      FeatureOptionList m_options;

      inline FeatureOptionList create( const std::string &name = "", 
                                       const std::vector<FeatureOption> &options = {} ) {
        
        std::string name_t = name;
        for ( const FeatureOption &opt : options ) {
          if ( opt.name() == "name" ) {
            name_t = opt.to_string();
            break;
          }
        }

        // Create the list
        FeatureOptionList feature_t( name_t, options );

        // Check for a valid name
        if ( feature_t.name().length() == 0 ) {
          // throw std::runtime_error( "ProductFeature must have a valid name!" );
        }
     
        // Ensure it has a name structure
        feature_t.add( FeatureOption( "name", feature_t.name() ) );

        return ( feature_t );
      }


      inline bool validate_file( const ProductFeature &other ) const {
        if (other.contains(other.name())) { //name-value becomes key that contains file ref
          std::vector<std::string> valid_wordlist = this->file_suffixes();
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
      
      inline bool validate_alias( const ProductFeature &other ) const {
        std::vector<std::string> alias_list = this->aliases();
        for (const auto &val : alias_list) {
          if (other.name() == val) {
            return true;
          }
        }
        return false;
      }
      
      inline bool validate_string( const ProductFeature &other ) const {
        if (other.contains(other.name())) {
          std::vector<std::string> valid_wordlist = this->value(other.name(), valid_wordlist);
          std::string target = psmrts_tolower(other.value(other.name(), std::string("")));

          for (const auto &vals : valid_wordlist) {
            if (target == psmrts_tolower(vals)) {
              return true;
            }
          }
        }
        return false;
      }

      inline FeatureOptionList from_json( const ordered_json &j_feature,
                                          const std::string &name = "feature" ) {
    
        // Assume its an object that contains keywords
        std::string name_t = name;
        std::vector<FeatureOption> options_t;
        for( auto &[ j_key, j_value ] : j_feature.items() ) {
          if ( "name" == j_key ) {
            name_t = j_value;
          }
            
          options_t.push_back( FeatureOption( j_key, j_value ) );
        }

        return ( FeatureOptionList( name_t, options_t) );
      }
  };
} // namespace psmrts

#endif
