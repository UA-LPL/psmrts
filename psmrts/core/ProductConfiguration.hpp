#pragma once
#ifndef ProductConfiguration_hpp
#define ProductConfiguration_hpp

#include <string>
#include <vector>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>

namespace psmrts { 


  /**
   * @brief Process/maintain user/dev product requests 
   * 
   * This class maintains user and developer configuration requests that are
   * submitted to PSMRTS.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-29 Kris J. Becker - Original Version
   */
  class ProductConfiguration {
    public:
      using ProductOptionList = std::vector<ProductOption>;

      ProductConfiguration( ) : m_name("none"), m_options{} { }
      ProductConfiguration(const std::string &name ) : m_name( psmrts_tolower(name) ), m_options{} { }
      explicit ProductConfiguration( const std::string &name, 
                                     const std::initializer_list<ProductOption> &options ) : 
                                     m_name( psmrts_tolower(name) ),
                                     m_options( options.begin(), options.end() ) { }                                
      explicit ProductConfiguration( const std::string &name, 
                                     const std::vector<ProductOption> &options ) : 
                                     m_name( psmrts_tolower(name) ),
                                     m_options( options ) { }      

      virtual ~ProductConfiguration() = default;


      inline const std::string &name() const {
        return ( m_name );
      }

      inline size_t size() const {
        return ( m_options.size() );
      }

      /** Deemed invalid if there are no options in this configuration */
      inline bool isValid() const {
        return ( this->size() > 0 );
      }

      /** Check for an existing option */
      inline bool contains( const std::string &name ) const {
        std::string name_l = psmrts_tolower( name );
        for ( const auto &option : m_options ) {
          if ( option.name() == name_l ) {
            return ( true );
          }
        }
        return ( false );
      }

      inline void add( const ProductOption &option ) {
        m_options.push_back( option );
        return;
      }

      inline const ProductOption &find( const std::string &name ) const {       
        std::string name_l = psmrts_tolower( name );
        for ( const auto &option : m_options ) {
          if ( option.name() == name_l ) {
            return ( option );
          }
        }  
        throw std::runtime_error( "*** ProductOption::get(" +  name +  " ) error - option not found" );
      }

      /** Remove the specifed option */
      inline bool remove( const std::string &name ) {   
        std::string name_l = psmrts_tolower( name );
        ProductOptionList::iterator opt_t = m_options.begin();

        while ( opt_t != m_options.end() ) {
          if ( opt_t->name() == name_l ) {
            m_options.erase( opt_t );
            return ( true );
          }
          ++opt_t;
        }  
        return ( false );
      }

      inline void clear() {
        m_options.clear();
      }

      inline psmrts_json to_json( ) const {
        psmrts_json option_j;
        for ( const auto &opt_j : m_options ) {
          option_j.update( opt_j.to_json() );
        }
        return ( option_j );
      }

      /**
       * @brief Return difference of this config and another config
       * 
       * This method will compare the contents of this object contents
       * of the config object argument. Note this does not actually
       * compare values of option, but determines options that exist in
       * "config" but not in this object. See the compare method.
       * 
       * @param config  Contains a single product config that is compared with
       *                  the options contained in this object.
       * @return ProductConfiguration Contains all options in "config" that are
       *                                 not also in this object 
       */
      inline ProductConfiguration difference( const ProductConfiguration &config,
                                              const bool twoway = false ) const {
        ProductConfiguration diff_c( config.name() );
        if ( &config != this ) {
          for ( const auto &opt_t : this->options() ) {
            if ( !config.contains( opt_t.name() ) ) {
              diff_c.add( opt_t );
            }
            else {
              if ( config.find( opt_t.name() ).to_string() != opt_t.to_string() ) {
                diff_c.add( opt_t );
              }
            }
          }

          if ( true == twoway ) { 
            // Now check config for options that don't exist in diff
            for ( const auto &conf_t : config.options() ) {
              if ( !this->contains( conf_t.name() ) ) {
                diff_c.add( conf_t );
              }
            }
          }
        }
        return ( diff_c );
      }

      inline bool compare( const ProductConfiguration &config,
                           const bool throw_errors = false ) const {
        std::string errors_t;
        std::string newline("");
        for ( const auto &opt_t : this->options() ) {
          try {
            if ( !( opt_t == config.find( opt_t.name() ) ) ) {
              std::string mess = newline + "Option " + opt_t.name() + " does not match!";
              errors_t += mess;
              newline = "\n";
            }
          }
          catch ( const std::runtime_error &e ) {
            // Doesn't exist
            std::string mess = newline + "*** RuntimeError: " + opt_t.name() + " - " + e.what();            
            errors_t += mess;
            newline = "\n";
          }
        }

        if ( errors_t.length() > 0 ) {
          if ( true == throw_errors ) {
            errors_t += newline + "*** ProductConfiguration::compare( " + config.name() + " with errors:";
            throw std::runtime_error( errors_t );
          }
          return ( false );
        }

        return ( true );
      }

      /** Return the list of options in this configuration */
      inline const ProductOptionList &options() const {
        return ( m_options );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->compare( conf ) );
      }

    private:
      std::string       m_name;
      ProductOptionList m_options;
  };

} // namespace psmrts

#endif
