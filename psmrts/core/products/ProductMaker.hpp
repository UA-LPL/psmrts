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
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/algorithms/VariantTraits.hpp>


namespace psmrts {

  // namespace of variant trait algorith,ms
  namespace traits_v = psmrts::algorithm::variants;

  /** 
   * @brief PSMRTS product maker class that creates all products 
   * 
   * This class provides support of all product variant types. It can extract
   * all the class variant specifications for a particular product class. These
   * specifications are used to verify user-based configurations and create a
   * variant from the configuration. This variant class is then instantiated
   * within the product and retained in this object for subsequent use.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  template <typename Product>
    class ProductMaker : public PsmrtsRequest {
      using Variants         = typename Product::Variants;
      using ProductSpecsList = PsmrtsContainer<ProductSpecification>;

      public:
        ProductMaker( ) : PsmrtsRequest( "ProductMaker" ),
                          m_cart( "Product" ),
                          m_specs( "specs" ),
                          m_product( std::nullopt ) { }
        ProductMaker( const std::string &name ) : 
                      PsmrtsRequest( name ),
                      m_cart( name ),
                      m_specs( "specs" ),
                      m_product( std::nullopt ) { }                        
        virtual ~ProductMaker() { }
    
        /** Determine validity of the maker (a constructed product exists) */
        inline bool isvalid() const {
          return ( ( this->error_count() == 0 ) && m_product.has_value() );
        }

        /** Returns the specfication of the constructed product */
        inline const ProductSpecsList &specifications() const {
          return ( m_specs );
        }

        /** Returns the constructed product if valid otherwise a default product */
        inline Product product() const {
          if ( this->isvalid() ) return ( m_product.value() );
          return ( Product() );  // This returns an non-value variant
        }

        /** Returns the product cart used to construct the product */
        inline const ProductCart &cart() const {
          return ( m_cart );
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
        inline ProductSpecsList get_product_specs( ) const {

          ProductSpecsList v_specs;
          auto v_indexes = traits_v::indexing_tuple<std::variant_size_v<Variants>>;
          traits_v::tuple_foreach( v_indexes, [&](auto I) { // Compile time index of variant
            using V = std::variant_alternative_t<I, Variants>;
            ProductSpecification s = V().product_specifications();  // Should be able to get w/o instantiation!
            if ( s.size() > 0 ) { // Check for featureless variant
              v_specs.add( s );
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

          m_specs.clear();
          m_product.reset();
          m_cart = ProductCart( conf.name() );
          auto v_indexes = traits_v::indexing_tuple<std::variant_size_v<Variants>>;
          traits_v::tuple_foreach( v_indexes, [&](auto I) { // Compile time index of variant
            if ( m_product.has_value() ) return;  // Check if product has been created
            using V = std::variant_alternative_t<I, Variants>;
            ProductSpecification s = V().product_specifications();  // Should be able to get w/o instantiation!

            if ( s.size() > 0 ) { // Check for featureless variant
              m_cart = ProductCart::extract_config( conf, s );

              if ( m_cart.isvalid() ) { 
                try {
                  m_product.emplace( Product( V( m_cart )) );
                  m_specs.add( s );
                  return;                

                }
                catch ( const std::runtime_error &re ) {
                  this->add_error( re );
                }
              }
            }
          } );
          
          return ( m_product.has_value() );
        }

        /**
         * @brief Process a product cart containing a product configuration
         * 
         * This method uses contents of a product cart to create a new product.
         * The cart contains a specifcation and a configuration that has been
         * preprocessed to be compatible with the product specification. 
         * 
         * This method searches the available variants for the compatible
         * config/specs and calls the cart constructor to create the product.
         * 
         * This method does not search any inventory for a compatible product.
         * This should be done prior to calling the maker.
         * 
         * @param cart    The config/spec data used to construct the product
         * @return true   If the product was successfully created
         * @return false  If the product could not nbe created
         */
        inline bool process_cart( const ProductCart &cart ) {

          m_specs.clear();
          m_product.reset();
          m_cart = cart;      

          auto v_indexes = traits_v::indexing_tuple<std::variant_size_v<Variants>>;
          traits_v::tuple_foreach( v_indexes, [&](auto I) { // Compile time index of variant
            if ( m_product.has_value() ) return;  // Check if product has been created

            using V = std::variant_alternative_t<I, Variants>;
            ProductSpecification s = V().product_specifications();  // Should be able to get w/o instantiation!
            if ( s.name() == cart.specification().name() ) {
              try {
                m_product.emplace( Product( V( m_cart )) );
                m_specs.add( s );
                return;
              }
              catch ( const std::runtime_error &re ) {
                this->add_error( re );
              }
            }
          } );           
          
          return ( m_product.has_value() );
        }


        /**
         * @brief Create a new product with a cart and one additional argument
         * 
         * This method is specialized to create a new product using a product
         * cart and an additional argument. It is designed specifically for
         * tracer variants that require a shape argument but may be useful for
         * other products as well.
         * 
         * The cart must contain a specific specification that is present in the
         * class template Product parameter. It searches through all the
         * variants to find the variant of the same name and then determines if
         * the variant has the approipriate Shape type. Shape can be anything
         * but this method is typically useful to construct a Bullet tracer with
         * a Shape = PsmrtsShape.
         * 
         * @tparam Shape Template type used as an additional argumemt in a constructor
         * @param cart   Product cart containing a config and specification
         *                 associated with a variant specification. It is
         *                 assumed to have a compatible constructor for the arguments.
         * @param shape  The Shape type argument passed into the variant constructor
         * @return true  If the product is successfully constructed
         * @return false If the product could not be constructed
         */
        template <typename Shape> 
          inline bool process_cart( const ProductCart &cart,
                                    const Shape &shape ) {
          m_specs.clear();
          m_product.reset();
          m_cart = cart;           
          auto v_indexes = traits_v::indexing_tuple<std::variant_size_v<Variants>>;
          traits_v::tuple_foreach( v_indexes, [&](auto I) { // Compile time index of variant
            if ( m_product.has_value() ) return;  // Check if product has been created
            using V = std::variant_alternative_t<I, Variants>;
            ProductSpecification s = V().product_specifications();  // Should be able to get w/o instantiation!
            if ( s.name() == cart.specification().name() ) {
              try {
                m_product.emplace( Product( traits_v::construct_compatible_product<V>( m_cart, shape ) ) );
                m_specs.add( s );
                return;
              }
              catch ( const std::runtime_error &re ) {
                this->add_error( re );
                this->add_error( "PsmrtsMaker::process_cart( cart, Args...) cannot be created for " + s.name() );
              }
            }
          } );           
          
          return ( m_product.has_value() );
        }

      private:
        ProductCart            m_cart;
        ProductSpecsList       m_specs;
        std::optional<Product> m_product;
    };

} // namespace psmrts

#endif
