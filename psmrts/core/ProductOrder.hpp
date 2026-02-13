/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductOrder_hpp
#define ProductOrder_hpp

#include <string>
#include <functional>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/ProductConfiguration.hpp>

namespace psmrts {


  /** 
   * @brief PSMRTS product order results obtained from specs/config processing
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  class ProductOrder : public PsmrtsRequest {
    public:
      using UIDType = PsmrtsUID::UIDType;

      ProductOrder( ) : PsmrtsRequest( "ProductOrder" ),
                        m_config( "ProductOrder" ),
                        m_residual( "ProductOrder" ),
                        m_trans( ),
                        m_dependencies(),
                        m_product_id( PsmrtsUID::null_uid() ) { }
      ProductOrder( const std::string &name,
                    const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                    PsmrtsRequest( name ),
                    m_config( name ),
                    m_residual( name ),
                    m_trans( trans ),
                    m_dependencies(),
                    m_product_id( PsmrtsUID::null_uid() ) { }
      ProductOrder( const ProductConfiguration &config,
                    const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                    PsmrtsRequest( config.name() ),
                    m_config( config ),
                    m_residual( config.name() ),
                    m_trans( trans ),
                    m_dependencies(),
                    m_product_id( PsmrtsUID::null_uid() ) {}
      ProductOrder( const ProductConfiguration &config,
                    const ProductConfiguration &residual,
                    const PsmrtsTranslations &trans = PsmrtsTranslations(), 
                    const std::vector<std::string> &depends = {} ) : 
                    PsmrtsRequest( config.name() ),
                    m_config( config ),
                    m_residual( residual ),
                    m_trans( trans ),
                    m_dependencies( depends ),
                    m_product_id( PsmrtsUID::null_uid() ) { }

      virtual ~ProductOrder() = default;

      inline bool isvalid() const {
        return ( ( m_config.size() != 0 ) && 
                 ( m_residual.size() == 0 ) && 
                 ( this->error_count() == 0) );
      }


      inline size_t size() const {
        return ( m_config.size() );
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline void add_option( const ProductOption &option ) {
        m_config.add( option );
      }

      inline void add_metadata( const ProductOption &option ) {
        m_config.add_metadata( option );
      }

      inline void merge_config( const ProductConfiguration &config ) {
        m_config.merge( config );
      }

      inline void set_config( const ProductConfiguration &config ) {
        m_config = config;
      }


      inline size_t residual_size() const {
        return ( m_residual.size() );
      }

      inline const ProductConfiguration &residual() const {
        return ( m_residual );
      }

      inline ProductConfiguration residual_dependencies() const {
        ProductConfiguration combo( m_residual.name(), m_residual.options() );
        for( const auto &option : m_residual.metadata() ) {
          combo.add( option );
        }

        return ( combo );
      }      

      inline void add_residual( const ProductOption &option ) {
        m_residual.add( option );
      }      
      

      inline const PsmrtsTranslations &translations() const {
        return ( m_trans );
      }

      inline void set_translations( const PsmrtsTranslations &translations ) {
        m_trans = translations;
      }

      inline std::string translate_path( const std::string &filepath ) const {
        return ( m_trans.translate_path( filepath ) );
      }

      inline size_t dependency_size() const {
        return ( m_dependencies.size()  );
      }
 
      inline bool has_dependencies() const {
        return ( m_dependencies.size() > 0  );
      }
 
      inline bool has_named_dependency( const std::string &name ) const {
        return ( std::find( m_dependencies.begin(), m_dependencies.end(), name ) != m_dependencies.end() );
      }
           
      inline void add_dependency( const std::string &name ) {
        m_dependencies.push_back( name );
      }

      inline void add_dependency( const ProductOption &option, 
                                  const std::string &alias = "" ) {
        std::string name_d = ( alias.length() > 0 ) ? alias : option.name();                                    
        if ( !has_named_dependency( name_d ) ) {
          this->add_dependency( name_d );
        }

        m_residual.add_metadata( option );
      }

      inline void add_dependencies( const std::vector<std::string> &depends_v ) {
        m_dependencies.insert( m_dependencies.end(), depends_v.begin(), depends_v.end() );
      }

      inline const std::vector<std::string> &dependencies( ) const {
        return ( m_dependencies );
      }


      inline bool has_uid() const {
        return ( PsmrtsUID::is_valid_uid( m_product_id ) );
      }

      inline UIDType uid() const {
        return ( m_product_id );
      }

      inline void set_uid( const UIDType &uid = PsmrtsUID::null_uid() ) {
        m_product_id = uid;
      }

      inline bool is_filled() const {
        return ( this->has_uid() ) ;
      }

    private:
      ProductConfiguration     m_config;
      ProductConfiguration     m_residual;
      PsmrtsTranslations       m_trans;
      std::vector<std::string> m_dependencies;
      UIDType                  m_product_id;
  };

      
} // namespace psmrts

#endif
