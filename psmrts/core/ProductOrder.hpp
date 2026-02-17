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
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/PsmrtsJson.hpp>

namespace psmrts {


  /** 
   * @brief PSMRTS product order results obtained from specs/config processing
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  class ProductOrder : public PsmrtsRequest {
    public:
      using ProductOptionList = ProductCart::ProductOptionList;
      using ResidualOptions   = ProductCart::ResidualOptions;

      ProductOrder( ) : PsmrtsRequest( "none" ),
                        m_submitted( ),
                        m_cart( ),
                        m_translations( ),
                        m_dependencies() { }
      ProductOrder( const std::string &name,
                    const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                    PsmrtsRequest( name ),
                    m_submitted( ),
                    m_cart( ),
                    m_translations( trans ),
                    m_dependencies() { }
      ProductOrder( const ProductConfiguration &submitted,
                    const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                    PsmrtsRequest( submitted.name() ),
                    m_submitted( submitted ),
                    m_cart( ProductSpecification(), submitted ),
                    m_translations( trans ),
                    m_dependencies() {}
      ProductOrder( const ProductConfiguration &submitted,
                    const ProductConfiguration &config,
                    const PsmrtsTranslations &trans = PsmrtsTranslations(), 
                    const std::vector<std::string> &depends = {} ) : 
                    PsmrtsRequest( submitted.name() ),
                    m_submitted( submitted ),
                    m_cart( ProductSpecification(), config ),
                    m_translations( trans ),
                    m_dependencies( depends ) { }                    
      ProductOrder( const ProductCart &product,
                    const PsmrtsTranslations &trans = PsmrtsTranslations(), 
                    const std::vector<std::string> &depends = {} ) : 
                    PsmrtsRequest( product.name() ),
                    m_submitted( product.configuration() ),
                    m_cart( product ),
                    m_translations( trans ),
                    m_dependencies( depends ) { }
      virtual ~ProductOrder() = default;

      inline size_t size() const {
        return ( m_cart.size() );
      }

      inline bool isvalid() const {
        return ( ( m_cart.has_valid_content() ) && 
                 ( this->error_count() == 0) );
      }

      inline const ProductConfiguration &submitted() const {
        return ( m_submitted );
      }

      inline const ProductCart &cart() const {
        return ( m_cart );
      }

      inline const PsmrtsTranslations &translations() const {
        return ( m_translations );
      }

      inline void set_translations( const PsmrtsTranslations &translations ) {
        m_translations = translations;
      }

      inline const ProductConfiguration &config() const {
        return ( m_cart.configuration() );
      }

      inline const ProductSpecification &specs() const {
        return ( m_cart.specification() );
      }

      inline void set_specification( const ProductSpecification &specs = ProductSpecification() ) {
        m_cart.set_specification( specs );
        return;
      }


      inline const ProductOptionList &options() const {
        return ( this->config().options()  );
      }

      inline void add_option( const ProductOption &option ) {
        m_cart.add_option( option );
      }

      inline void add_metadata( const ProductOption &option ) {
        m_cart.add_metadata( option );
      }

      inline void set_config( const ProductConfiguration &config ) {
        m_cart.set_configuration( config );
      }

      inline size_t residual_size() const {
        return ( m_cart.residual().size() );
      }

      inline const ResidualOptions &residual() const {
        return ( m_cart.residual() );
      }

      inline ProductConfiguration residual_config() const {
        return ( ProductConfiguration( this->name(), this->residual() ) );
      }      

      inline void add_residual( const ProductOption &option ) {
        m_cart.add_residual( option );
      }      
      
      inline std::string translate_path( const std::string &filepath ) const {
        return ( m_translations.translate_path( filepath ) );
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

        this->add_residual( option );
      }

      inline void add_dependencies( const std::vector<std::string> &depends_v ) {
        m_dependencies.insert( m_dependencies.end(), depends_v.begin(), depends_v.end() );
      }

      inline const std::vector<std::string> &dependencies( ) const {
        return ( m_dependencies );
      }

      inline ordered_json to_json() const {
        ordered_json order_j = ordered_json::object();
        order_j.update( json_utils::insert_object( "submitted", m_submitted.to_json() ) );
        order_j.update( json_utils::insert_object( "product",   m_cart.to_json() )  );
        order_j["dependencies"] = m_dependencies;
        return ( order_j );
      }

      inline ProductOrder make_composite( const ProductOrder &other ) const {

        // Make a copy of the current order
        ProductOrder order_t = *this;

        // Append this->options() to other options to this one and replace the
        // cart configuration with this new list whilst preserving the metadata
        // in this config data.
        auto config_t = ProductConfiguration( this->config().name(), 
                                              other.config().options(),
                                              this->config().metadata() );
        for ( const auto &option : this->config().options() ) {
          config_t.add( option );
        }

        // Now replace only the cart with the processed combined config
        order_t.m_cart = ProductCart( this->specs(), config_t, this->residual() );
        return ( order_t );
      }

    private:
      ProductConfiguration     m_submitted;
      ProductCart              m_cart;
      PsmrtsTranslations       m_translations;
      std::vector<std::string> m_dependencies;

  };

      
} // namespace psmrts

#endif
