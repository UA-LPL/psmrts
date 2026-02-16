/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsInvoice_hpp
#define PsmrtsInvoice_hpp

#include <string>
#include <functional>
#include <optional>
#include <tuple>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/algorithms/VariantTraits.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/ProductMaker.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>


namespace psmrts {

  /** 
   * @brief PSMRTS product order 
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  class PsmrtsInvoice : public PsmrtsProduct {
    public:
      using ProductOrderList = PsmrtsContainer<ProductOrder>;

      PsmrtsInvoice( ) : PsmrtsProduct( "PsmrtsInvoice" ),
                        m_orders(  ),
                        m_processor( ),
                        m_inventory() { }
      PsmrtsInvoice( const std::string &name,
                     const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                     PsmrtsProduct( name ),
                     m_orders( ),
                     m_processor( trans ),
                     m_inventory() { }                        
      virtual ~PsmrtsInvoice() { }
  

      inline size_t size() const {
        return ( m_orders.size() );
      }


      inline const PsmrtsTranslations &translations() const {
        return ( m_processor.translator() );
      }

      inline bool add_product( const ProductConfiguration &config ) {
        ProductOrder order_t = process_product( config );
        if ( order_t.error_count() > 0 ) {
          std::string mess = "PsmrtsInvoice::add_product(" + config.name() +
                             ") errors occured during validation: " +
                             order_t.errors_to_string();
          throw std::runtime_error( mess );
        }
        
        if ( !order_t.isvalid()  ) {
          auto residuals = order_t.residual_config();
          std::string resid_s = residuals.to_json( residuals.options() ).dump(-1);
          std::string mess = "PsmrtsInvoice::add_product(" + config.name() +
                             ") is not valid, has unrecognized key/value options: " +
                             resid_s;
          throw std::runtime_error( mess );
        }

        // Add it to the invoice
        m_orders.add( order_t );
        return ( true );

      }

      inline const ProductOrderList &orders() const {
        return ( m_orders );
      }


      /**
       * @brief Process a configuration returning a product order with status
       * 
       * This method function takes a compound configuration and returns
       * results that are intended to create 
       * 
       * @param config 
       * @param translations 
       * @return ProductOrder 
       */
      inline ProductOrder process_product( const ProductConfiguration &config ) { 
        ProductOrder order_t( config.name() );                              
        if ( config.size() == 0 ) return ( order_t );

        // Process the till the first occurance of valid or no errors occurs
        auto tracer_specs_v = ProductMaker<PsmrtsTracer>().get_product_specs();
        for ( const auto &tracer_s : tracer_specs_v ) {
          order_t = m_processor.process_cart( ProductCart( tracer_s, config) );
          order_t.add_dependency( tracer_s.name() );

          // If this parse is successful, we are done and its a standalone tracer.
          if ( order_t.isvalid() ) {
            return ( order_t );
          }

          // Check for errors. If none break for shape processing
          if ( order_t.error_count() == 0 ) {
            break;
          }
        }
        
        // If we have errors, then no tracer is detected/valid for this config
        // and we only have a shape to consider. Pass the orginal config for
        // shape processing. 
        //
        // If we have no errors but its not valid, assume a shape is required
        // and copy the residual config and process shape.
        ProductConfiguration config_t( config.name()  );
        ProductOrder order_s( config.name() );
        if ( ( order_t.error_count() > 0 ) || ( order_t.size() == 0 )) {
          // Process as shape only, start over
          config_t = config;
        }
        else {
          // order_t content contains processed tracer, lets see if we have
          // shape to consume the remaining residual/dependencies
          config_t = order_t.residual_config();
        }

        auto shape_specs_v  = ProductMaker<PsmrtsShape>().get_product_specs();
        for ( const auto &shape_s : shape_specs_v ) {
          order_s = m_processor.process_cart( ProductCart( shape_s, config_t ) );
          if ( order_s.isvalid() ) break;
        }

        // Make it all make sense. Retain errors and status in the composite
        // version.
        order_t = order_s.make_composite( order_t );

        return ( order_t );
      }

    private:
      ProductOrderList               m_orders;
      ProductProcessing              m_processor;
      std::optional<PsmrtsInventory> m_inventory;
  };

} // namespace psmrts

#endif
