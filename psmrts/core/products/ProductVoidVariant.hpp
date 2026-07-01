/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductVoidVariant_hpp
#define ProductVoidVariant_hpp

#include <vector>
#include <deque>
#include <string>
#include <exception>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>


namespace psmrts { 

  /**
   * @brief A specialized void type variant placeholder for monostate
   * 
   * 
   * @see PsmrtsTracer.hpp for an example of this technique.
   */
  class ProductVoidVariant : public MissingProcessRequestHandler {
    public:
      ProductVoidVariant() : MissingProcessRequestHandler( "void" ),
                             m_config( "void" ),
                             m_specs( "void", "variant" ) { }
      ProductVoidVariant( const std::string &name ):
                          MissingProcessRequestHandler( "void" ),
                          m_config( name ),
                          m_specs( name, "variant" ) { }
      ProductVoidVariant( const ProductCart &processed_cart ) :
                          MissingProcessRequestHandler( "void" ),
                          m_config( processed_cart.name() ),
                          m_specs( processed_cart.name(), "variant" ) {
        this->create( processed_cart );
      }
      virtual ~ProductVoidVariant() = default;
      
      static inline ProductSpecification product_specifications() {
        return ( ProductSpecification( "void", "variant" ) );
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( false );
      }

      inline double maximum_radius() const {
        return ( psmrts::null() );
      }

      inline double minimum_radius() const {
        return ( psmrts::null() );
      }
      
    private:
      ProductConfiguration m_config;
      ProductSpecification m_specs;

      inline void create( const ProductCart &processed_cart ) {
        throw std::runtime_error( "PsmrtsVoidVariant is not a valid product!");
      }


  };

} // namespace psmrts

#endif
