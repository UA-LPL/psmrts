/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsTranslations_hpp
#define PsmrtsTranslations_hpp

#include <exception>
#include <string>
#include <cstdio>
#include <tuple>

// Setup for environment variable support
#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
#define NOMINMAX
#include <windows.h>
#else
  extern "C" char **environ;
#endif
#include <algorithm>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>

namespace psmrts {

  /**
   * @brief PSMRTS translations using environment and parameter replacements 
   * 
   * This class sets up a system of path-like substitutions that start with a
   * "$" and are present in a file path string. There are two types of path
   * variables supported in the class. 
   * 
   * One is the system path environment variable as maintained in the
   * Linux/Windows shell environment. For /bin/bash these are created using the
   * "export variable=value". Both are variable and value are interpreted as
   * strings with single values. These variables are case sensitive.
   * 
   * The second is a parameter of the same format as the enviroment variable but
   * the paramater varible is case insensitive. This is mainly to support
   * ISIS-like DataDirectory group key/value pairs that can have mixed case.
   * These variables will be converted automatically to lower case by the
   * container that stores these variables.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-02-04 Kris J. Becker  Original Version
  */
  class PsmrtsTranslations {
    public:
      using ParameterInventory   = PsmrtsCache<std::string, std::string, CompareCaseInsensitive>;
      using EnvironmentInventory = PsmrtsCache<std::string, std::string, CompareCaseInsensitive>;

      PsmrtsTranslations(  ) {
        this->init( "translations" );
      }
      PsmrtsTranslations( const std::string &name, const bool load_env = false  ) {
        this->init( name );
        if (load_env) {
          this->load_and_merge_environment( );
        }
      }
      PsmrtsTranslations( const PsmrtsTranslations &trans) = default;
      virtual ~PsmrtsTranslations() = default;

      static PsmrtsTranslations create( const std::string &name = "translations" ) {
        PsmrtsTranslations trans_t ( name );
        trans_t.load_and_merge_environment( );
        return ( trans_t );
      }

      inline size_t size() const {
        return ( m_parameters.size() + m_environment.size() );
      }

      inline bool empty( ) const {
        return ( this->size() == 0 );
      }

      /** Return the name of this environment */
      inline const std::string &name () const {
        return ( m_name );
      }

      /** Return parameter inventory set */
      inline const ParameterInventory &parameters( ) const {
        return ( m_parameters );
      }        

      /** Return environment variables set */
      inline const EnvironmentInventory &environment( ) const {
        return ( m_environment );
      }        

      /**
       * @brief Translate file path with names of enviroments in the system 
       * 
       * This method will fully translate a mixture of parameter and environment
       * variables contained within the filepath. Each parameter or environment
       * variable begins with a '$' character. The variable ends with a '/'
       * character. The algorithm checks the parameter variable cache then
       * environment variables are check.
       * 
       * Parameter variables are case insenstive. Environment variables are case
       * sensitive.
       * 
       * File paths (or any string really) can contain parameters or environment
       * variables that also contain a parameter/environment variable. The
       * "iterations" parameters specifies how many iterations to process
       * embedded variables. One each iteration only one
       * translation/substitution will occur with the parameters checked first.
       * 
       * @param filepath The string containing a path assumed to contains a
       *                   parameter/environment variable.
       * @param iterations The number of iterations to check for variable
       *                     subsititutions. Default = 5.
       * @return std::string Returns the filepath with varaiable substitutions 
       */
      inline std::string translate_path(const std::string &filepath, 
                                        const size_t iterations = 5 ) const {
        
        std::string path = filepath;
        for ( size_t n = 0 ; n < iterations ; n++ ) {
          std::string path_t = this->substitute_path_environment( path );
          if ( path_t == path ) break;
          path = path_t;
        }
        return ( path );
      }

      /**
       * @brief Substitute environment strings with environent-type variables
       * 
       * This template method is will search for all occurances of '$' in the
       * string and assume it is an environment or parameter variable
       * reference that should be replaced with the value of the variable.
       * 
       * This method expects a specific path structure where at least one
       * element delimited by '/' characters begins with a '$' and ends at the
       * slash. The '$' is stripped off with the remaining text content check if
       * it exists in the "envirnment" lookup path.
       * 
       * The supported pattern of a file specfication is:
       * 
       * @code}
       *   $osirisrex/kernels/dsk/bennu512.bds
       * @endcode
       * 
       * In this case, once a single '$' is detected in the string, the string
       * is tokenized by file path seperators, both forward and backward
       * slashes. Each token is examined for the '$' in the string a substring
       * from the first character after the '$' to the end of the token string
       * is tested in the class containing a map of a 'parameter' variable and
       * then an 'environment' variable. If it exists, the entire substring is
       * replaced with the value of the parameter/environment variable. When all
       * the tokens are examined, the tokens are then rejoined into a new 
       * string with a '/' path separator. 
       * 
       * @param s    String containing path to parse and translate
       * @param env  Environment cache to use for translations
       * @return std::string Translated path free of parameters/environment
       *                       variables
       */
      inline std::string substitute_path_environment( const std::string &s ) const {
        if ( s.length() == 0 ) return ( s );
        if ( s.find_first_of( '$') == std::string::npos ) return ( s );

        std::vector<std::string> dirs = psmrts::string_tokenizer( s, "/\\" );
        std::vector<std::string> path_t;
        for ( std::string sub_d : dirs ) {
          auto index_d = sub_d.find_first_of('$');
          if ( std::string::npos != index_d ) {
            std::string env_t = sub_d.substr(index_d+1, std::string::npos );
            if  (this->parameters().contains( env_t ) ) {
              std::string str_r = this->parameters().find( env_t );
              sub_d.replace(sub_d.begin()+index_d, str_r.end(), str_r );
            }            
            else if ( this->environment().contains( env_t ) ) {
              std::string str_r = this->environment().find( env_t );
              sub_d.replace(sub_d.begin()+index_d, str_r.end(), str_r );
            }

          }
          path_t.push_back( sub_d );
        }
        
        // Join the path with unix string separators
        std::string delim_c =  ( s[0] == '/' ) ? "/" : "";
        std::ostringstream oss;
        for ( const std::string  &s_t : path_t ) {
          oss << delim_c << s_t;
          delim_c = "/";
        }

        return ( oss.str() );
      }
      
      /** Merge a PsmrtsTranslations into this inventory */
      inline size_t merge( const PsmrtsTranslations &other ) {
        size_t n_merged = 0;
        n_merged += m_parameters.merge( other.parameters() );
        n_merged += m_environment.merge( other.environment() );
        return ( n_merged );
      }

      inline size_t merge_parameters( const ParameterInventory &parameters ) {
        return ( m_parameters.merge( parameters ) );
      }

      inline size_t merge_environment( const EnvironmentInventory &environment ) {
        return ( m_environment.merge( environment ) );
      }      

      inline void add_parameter( const std::string &name, 
                                 const std::string &parameter ) {
        m_parameters.add( name, parameter );
      }

      /** Remove a keyword from the parameter set */
      inline void remove_parameter( const std::string &name ) {
        m_parameters.remove( name );
      }

      inline void add_environment( const std::string &envname, 
                                  const std::string &parameter ) {
        m_environment.add( envname, parameter );
      }

      inline void remove_environment( const std::string &envname ) {
        m_environment.remove( envname );
      }

      /* Creates an inventory of environment variables from current state */
      static inline EnvironmentInventory create_environment( const std::string &name_p = "environment ") {
        return ( PsmrtsTranslations::get_environment_variables( name_p ) );
      }

      /** Returns an empty parameters Inventory */
      static inline ParameterInventory create_parameters( const std::string &name_p = "parameters ") {
        return ( ParameterInventory( name_p ) );
      }

    private:
      std::string          m_name;
      ParameterInventory   m_parameters;
      EnvironmentInventory m_environment;
      
      
      /** Reinitialize everything  */
      inline void init( const std::string &name = "translations" ) {
        m_name = name;
        m_parameters.clear();
        m_environment.clear();
      }

      inline const EnvironmentInventory &load_and_merge_environment( ) {
        m_environment.merge( PsmrtsTranslations::get_environment_variables( ) );
        return ( m_environment );
      }

      static inline std::tuple<std::string,std::string> parse_env_string( const std::string &env_s ) {
        size_t eq_pos = env_s.find( "=" );
        if ( std::string::npos != eq_pos ) {
          return ( std::make_tuple( env_s.substr(0, eq_pos), env_s.substr( eq_pos+1, std::string::npos ) ) );
        }
        else {
          return ( std::make_tuple( env_s, std::string("") ) );
        }
      }

            
      /** Load all the environment variables */
      static inline EnvironmentInventory get_environment_variables( const std::string &name_p = "environment" ) {
          EnvironmentInventory env_t = EnvironmentInventory( name_p ) ;

#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
          // **** Windows implementation *****/
          LPCH envStrings = GetEnvironmentStringsA();
          if ( nullptr == envStrings ) {
            return ( env_t );
          }

          LPCH env = envStrings;
          while ( *env != '\0' ) {
            std::string env_entry(env);
            auto [ key, value ] = PsmrtsTranslations::parse_env_string( env_entry );            
            env_t.add(key, value);

            env += strlen(env) + 1;
          }
          
            FreeEnvironmentStringsA(envStrings);           
#else
          // **** Linux implementation *****/
          char **env = environ;
          while ( *env != nullptr ) {
            auto [ key, value ] = PsmrtsTranslations::parse_env_string(  *env );
            env_t.add( key, value );
            env++;
          }
#endif
          return ( env_t );
        }

  };

  // Declare a shared pointer type for translations
  using SharedTranslations = std::shared_ptr<PsmrtsTranslations>;

} // namespace psmrts

#endif
