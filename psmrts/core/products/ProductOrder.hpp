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
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>

namespace psmrts {


  /** 
   * @brief PSMRTS product order results obtained from specs/config processing
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  class ProductOrder : public PsmrtsErrors {
    public:
      using CartCache         = PsmrtsSharedCache<std::string, ProductCart, CompareCaseInsensitive>;
      using ProductOptionList = ProductCart::ProductOptionList;
      using ResidualOptions   = ProductCart::ResidualOptions;

      ProductOrder( ) : PsmrtsErrors( ),
                        m_submitted( ),
                        m_carts( ),
                        m_translator() { }
      ProductOrder( const std::string &name,
                    const SharedTranslations &trans = SharedTranslations() ) : 
                    PsmrtsErrors( ),
                    m_submitted( ),
                    m_carts( name ),
                    m_translator( trans ) { }
      ProductOrder( const ProductConfiguration &submitted,
                    const SharedTranslations &trans = SharedTranslations() ) : 
                    PsmrtsErrors( ),
                    m_submitted( submitted ),
                    m_carts( submitted.name() ),
                    m_translator( trans ) {

      }
      ProductOrder( const ProductConfiguration &submitted,
                    const ProductConfiguration &config,
                    const SharedTranslations &trans = SharedTranslations() ) : 
                    PsmrtsErrors( ),
                    m_submitted( submitted ),
                    m_carts( submitted.name() ),
                    m_translator( trans )  { }                    
      ProductOrder( const ProductCart &cart,
                    const SharedTranslations &trans = SharedTranslations() ) : 
                    PsmrtsErrors( ),
                    m_submitted( cart.configuration() ),
                    m_carts(  ),
                    m_translator( trans )  { 
        m_carts.add( cart.product(), cart );
      }
      ProductOrder( const ProductConfiguration &submitted,
                    const ProductCart &cart,
                    const SharedTranslations &trans = SharedTranslations() ) : 
                    PsmrtsErrors( ),
                    m_submitted( submitted ),
                    m_carts( submitted.name() ),
                    m_translator( trans )  { 
        m_carts.add( cart.product(), cart );
      }                    
      virtual ~ProductOrder() = default;

      inline size_t size() const {
        return ( m_carts.size() );
      }

      inline const std::string &name() const {
        return ( m_submitted.name() );
      }
      
      inline bool isempty() const {
        return ( m_carts.size() == 0 );
      }

      inline bool isvalid() const {
        return ( !this->isempty() && ( this->error_count() == 0 ) );
      }

      inline const ProductConfiguration &config() const {
        return ( m_submitted );
      }

      inline void add( const ProductCart &cart ) {
        m_carts.add( cart.product(), cart );
        return;
      }

      inline SharedCart find( const std::string &cart_type ) const {
        return ( m_carts.find( cart_type ) );
      }

      inline std::vector<std::string> cart_keys() const {
        return ( m_carts.keys() );
      }

      inline std::vector<SharedCart> cart_values() const {
        return ( m_carts.values() );
      }

      inline SharedTranslations translations() const {
        return ( m_translator );
      }

      inline void set_translations( const SharedTranslations &translations ) {
        m_translator = translations;
      }

      inline std::string translate_path( const std::string &filepath ) const {
        if ( !m_translator ) return ( filepath );
        return ( m_translator->translate_path( filepath ) );
      }

#if 0      
      inline ordered_json to_json() const {
        ordered_json order_j = ordered_json::object();
        order_j.update( json_utils::insert_object( "submitted", m_submitted.to_json() ) );
        order_j.update( json_utils::insert_object( "product",   m_cart.to_json() )  );
        // order_j["dependencies"] = m_dependencies;
        return ( order_j );
      }
#endif

    private:
      ProductConfiguration m_submitted;
      CartCache            m_carts;
      SharedTranslations   m_translator;
      // PsmrtsInventory          m_inventory;

  };

  using SharedProductOrder = std::shared_ptr<ProductOrder>;
      
} // namespace psmrts

#endif
