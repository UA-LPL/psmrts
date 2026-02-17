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
#include <psmrts/core/PsmrtsFactory.hpp>


namespace psmrts {

  /** 
   * @brief PSMRTS invoice handles all product managements processes
   * 
   * This class accepts product configurations and generates products. It
   * maintains a local inventory containing all products while also utilizing
   * the PSMRTS factory for resource reuse. 
   * 
   * The ultimate product generated from this invoice is a priority tracer.
   * Although you can also produce other products that can be added to the
   * factory or reused for other purposes.
   * 
   * Each configuration will result in at most one tracer and potentially a
   * shape. Each configuration that results in a tracer will be part of the
   * priority tracer. 
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  class PsmrtsInvoice : public PsmrtsProduct, public PsmrtsRequest {
    public:
      using ProductSet       = ProductProcessing::ProductSet;
      using ProductOrderList = PsmrtsContainer<ProductSet>;


      PsmrtsInvoice( ) : PsmrtsProduct( "PsmrtsInvoice" ),
                         PsmrtsRequest( "invoice_errors" ),
                         m_orders(  ),
                         m_processor( ),
                         m_inventory( std::nullopt ) { }
      PsmrtsInvoice( const std::string &name,
                     const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                     PsmrtsProduct( name ),
                     PsmrtsRequest( "InvoiceErrors" ),                     
                     m_orders( ),
                     m_processor( trans ),
                     m_inventory( std::nullopt ) { }                        
      virtual ~PsmrtsInvoice() { }
  

      inline size_t size() const {
        return ( m_orders.size() );
      }

      inline const std::string &name() const {
        return ( PsmrtsProduct::name() );
      }

      inline bool has_inventory() const {
        return ( m_inventory.has_value() );
      }

      inline const PsmrtsTranslations &translations() const {
        return ( m_processor.translator() );
      }

      inline bool add_product( const ProductConfiguration &config ) {
        ProductSet product_s = m_processor.process_configuration( config );
        if ( !m_processor.is_valid_product( product_s ) ) {
          std::string mess = "PsmrtsInvoice::add_product(" + config.name() +
                             ") errors occured during validation: " +
                             m_processor.product_error_string( product_s );
          this->add_error( mess );
          return ( false );
        }
      
        // Add it to the invoice
        m_orders.add( product_s );
        return ( true );
      }

      /** Get a list of all the orders in this invoice */
      inline const ProductOrderList &orders( ) const {
        return ( m_orders );
      }

      /**
       * @brief Generate products from the orders contained in this invoice
       * 
       * If there is no inventory allocated (see has_inventory()) then one is
       * created and the orders are processed. Part of the processing is to use
       * the local inventory and factory inventory to find products to reuse.
       * If tracers or shapes are not found, then new ones are created from 
       * the configurations and added to the local cache added to the factory
       * cache. 
       * 
       * The local PsmrtsRequest error tracking mechanism is used to report any
       * issues that occur when creating products.
       * 
       * @return true  If all products are created successfully
       * @return false If product creation fails
       */
      inline bool generate_products( ) {

        return ( true );
      }

      /**
       * @brief Get the priority tracer object generated from the product configs
       * 
       * This method will create a priority tracer from the results of the
       * configs contained in this invoice. Its possible the products have not
       * been generated yet so the generate_products() method should have been
       * called prior to calling this routine - it is renentrant.
       * 
       * If one is not found or products have not been generated yet (indicated
       * by the method has_inventory()) 
       * 
       * @param name 
       * @return PsmrtsPriorityTracer 
       */
      inline PsmrtsPriorityTracer get_priority_tracer( const std::string &name = "" ) {

        std::string name_t = ( name.length() > 0 ) ? name : this->name();
        PsmrtsPriorityTracer tracer_p( name );

        // First search the local inventory if one exists
        if ( m_inventory.has_value() ) {
          tracer_p = this->find_priority_tracer( m_inventory.value(), name );
        }

        // If its not valid, it wasn't found so try the factory inventory
        if ( !tracer_p.isValid() ) {
          const auto &inventory_pt = PsmrtsFactory().find( );
          tracer_p = this->find_priority_tracer(inventory_pt, name );
        }

        return ( tracer_p );
      }

    private:
      ProductOrderList               m_orders;
      ProductProcessing              m_processor;
      std::optional<PsmrtsInventory> m_inventory;

      inline PsmrtsPriorityTracer find_priority_tracer( const PsmrtsInventory &inventory,
                                                        const std::string &name ) 
                                                        const {
        const auto &inventory_pt = inventory.prioritytracers();
        for ( const auto &pt : inventory_pt.cache() ) {
          if ( pt.second.name() == name ) {
            return ( pt.second );
          }       
        }

        // Returns an empty tracer indicating it wasn't found
        return ( PsmrtsPriorityTracer( name ) );
      }
  };

} // namespace psmrts

#endif
