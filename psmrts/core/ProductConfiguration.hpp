/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductConfiguration_hpp
#define ProductConfiguration_hpp

#include <string>
#include <vector>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/AllOptionConversions.hpp>

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
      using ContainerType     = PsmrtsContainer<ProductOption>;
      using ProductOptionList = ContainerType;
      using ProductMetadata   = ContainerType;

      ProductConfiguration( ) : m_identifier("undefined"), 
                                m_options( "options"),
                                m_metadata( "metadata" ) { }
      ProductConfiguration( const std::string &cid ) :
                            m_identifier( cid ), 
                            m_options( "options" ), 
                            m_metadata( "metadata" ) { }
      ProductConfiguration( const std::string &cid,
                            const ProductConfiguration &config ) :
                            m_identifier( cid ), 
                            m_options( config.options() ), 
                            m_metadata( config.metadata() ) { }                            
      ProductConfiguration( const std::string &cid,
                            const ProductOptionList &info ) :
                            m_identifier( cid ), 
                            m_options( info ), 
                            m_metadata( "metadata" ) { }                            
      explicit ProductConfiguration( const std::string &cid, 
                                     const std::initializer_list<ProductOption> &options ) : 
                                     m_identifier( cid ),
                                     m_options( "options", options ),
                                     m_metadata( "metadata" ) { }                                
      explicit ProductConfiguration( const std::string &cid, 
                                     const std::vector<ProductOption> &options ) : 
                                     m_identifier( cid ),
                                     m_options( "options", options ),
                                     m_metadata( "metadata" ) { }      

      virtual ~ProductConfiguration() = default;

      inline const std::string &name() const {
        return ( m_identifier );
      }

      inline size_t isvalid() const {
        return ( m_options.size()  > 0 );
      }

      inline size_t size() const {
        return ( m_options.size() );
      }

      inline const ProductOptionList &options( ) const {
        return ( m_options );
      }

      inline const ProductMetadata &metadata( ) const {
        return ( m_metadata );
      }      


      /** Check for an existing option, not including metadata */
      inline bool contains( const std::string &name ) const {
        return ( m_options.contains( name  ) );
      }

      inline const ProductOption &find( const std::string &name ) const {       
        return ( m_options.find( name ) );
      }

     inline void add( const ProductOption &option ) {
        m_options.replace( option );
        return;
      }

      inline void add_option( const ProductOption &option ) {
        m_options.replace( option );
        return;
      }
      
      inline void add_metadata( const ProductOption &option ) {
        m_metadata.replace( option );
        return;
      }

      inline void add_metadata( const ProductMetadata &metadata ) {
        for ( const auto &m : metadata.data() ) {
          m_metadata.replace( m );
        }
        return;
      }
      
      inline ProductConfiguration &merge( const ProductConfiguration &other ) {
        for ( const ProductOption &option_t : other.options() )  {
          m_options.replace( option_t );
        }

        for ( const ProductOption &meta_t : other.metadata() ) {
          m_metadata.replace( meta_t );
        }

        return ( *this );
      }

      inline const ProductOption &find_option( const std::string &name ) const {       
        return ( m_options.find( name ) );
      }

      inline const ProductOption &find_metadata( const std::string &name ) const {       
        return ( m_metadata.find( name ) );
      }

      inline bool remove( const std::string &name ) {
        return ( m_options.remove( name ) );
      }

      inline bool remove_metadata( const std::string &name ) {
        return ( m_metadata.remove( name ) );
      }

      inline ordered_json to_json( ) const {
        ordered_json j_opts = this->to_json( this->options() );
        if ( this->metadata().size() > 0 ) {
          j_opts["metadata"].update( to_json( this->metadata() ) );
        }
        return ( j_opts );
      }

      inline ordered_json to_json( const ContainerType &c ) const {

        ordered_json j_opts;
        for ( const auto &opt_t : c ) {
          j_opts.update( opt_t.to_json() );
        }
        return ( j_opts );
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
          if ( !config.contains( opt_t.name() ) ) {
            std::string mess = newline + "Option " + opt_t.name() + " does not match!";
            errors_t += mess;
            newline = "\n";
          }
          else {
            if ( !OptionStringsComparator::compare( opt_t, config.find( opt_t.name() ) ) ) {
              std::string mess = newline + "Option values in " + opt_t.name() + " do not match!";
              errors_t += mess;
              newline = "\n";
            }
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

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->compare( conf ) && conf.compare( *this) );
      }

    private:
      std::string       m_identifier;
      ProductOptionList m_options;
      ProductMetadata   m_metadata;
  };

} // namespace psmrts

#endif
