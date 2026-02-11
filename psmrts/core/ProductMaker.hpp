/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductMaker_hpp
#define ProductMaker_hpp

#include <string>
#include <functional>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/algorithms/VariantTraits.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>


namespace psmrts {

  // namespace of variant trait algorith,ms
  namespace traits_v = psmrts::algorithm::variants;

  /** 
   * @brief PSMRTS product order 
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  template <typename Product>
    class ProductMaker : public PsmrtsRequest {
      using Variants = typename Product::Variants;

      public:
        ProductMaker( ) : PsmrtsRequest( "ProductMaker" ),
                          m_order( "Product" ),
                          m_product( nullptr ) { }
        ProductMaker( const std::string &name ) : 
                      PsmrtsRequest( name ),
                      m_order( name ),
                      m_product() { }                        
        virtual ~ProductMaker() { }
    
        inline bool isvalid() const {
          return ( m_product.has_value() );
        }

        inline Product product() const {
          if ( this->isvalid() ) return ( m_product.value() );
          return ( Product() );  // This returns an non-value variant
        }

        inline const ProductOrder order() const {
          return ( m_order );
        }

        /**
         * @brief Get all valid  product specifications in product
         * 
         * A valid ProductSpecification is one that has at least one product
         * feature. Only specs with more than one feature is added to the list.
         * 
         * @return std::vector<ProductSpecification> Vector of all valid product
         *            specifications
         */
        inline std::vector<ProductSpecification> get_product_specs( ) const {

          std::vector<ProductSpecification> v_specs;
          auto v_indexes = traits_v::indexing_tuple<std::variant_size_v<Variants>>;
          traits_v::tuple_foreach( v_indexes, [&](auto I) { // Compile time index of variant
            using V = std::variant_alternative_t<I, Variants>;
            ProductSpecification s = V().product_specifications();  // Should be able to get w/o instantiation!
            if ( s.size() > 0 ) { // Check for featureless variant
              v_specs.push_back( s );
            }
          } );

          return ( v_specs );
        }
        
        /**
         * @brief Identify and create a products in a variant set given a configuration
         * 
         * This method iterates through all variants in a prouduct set and
         * identifies the variant that satisifies 
         * 
         * @tparam Registrar Registration source, typically PsmrtsFactory but can
         *                     be any type with a register_product() method. 
         * @param registrar  Registrar object get the ProductSpecification
         */
        inline bool process_config( const ProductConfiguration &conf,
                                    const PsmrtsTranslations &translations ) {

          m_product.reset();
          m_order = ProductOrder( conf.name() );
          auto v_indexes = traits_v::indexing_tuple<std::variant_size_v<Variants>>;
          traits_v::tuple_foreach( v_indexes, [&](auto I) { // Compile time index of variant
            if ( m_product.has_value() ) return;  // Check if product has been created
            using V = std::variant_alternative_t<I, Variants>;
            ProductSpecification s = V().product_specifications();  // Should be able to get w/o instantiation!
            // std::cout << "\nMakerSpecName: " << s.name() << std::endl;

            if ( s.size() > 0 ) { // Check for featureless variant
              m_order  = s.process_order( conf, translations );
              // std::cout << "ProductMaker OrderConfig:   " << m_order.config().to_json().dump(2) << std::endl;
              // std::cout << "ProductMaker OrderResidual: " << m_order.residual().to_json().dump(2) << std::endl;
              if ( m_order.isvalid() ) { 
                // std::cout << "Making Product Type: " << s.name() << std::endl;
                m_product.emplace( Product( V( m_order.config() )) );
                return;
              }
            }
          } );
          
          return ( m_product.has_value() );
        }

      private:
        ProductOrder           m_order;
        std::optional<Product> m_product;
    };

} // namespace psmrts

#endif
