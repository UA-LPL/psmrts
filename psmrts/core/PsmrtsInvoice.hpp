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

#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/algorithms/VariantTraits.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/products/ProductInventory.hpp>
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
      using UIDType              = PsmrtsProduct::UIDType;
      using ProductSet           = ProductProcessing::ProductSet;
      using ProductOrderList     = PsmrtsContainer<ProductSet>;
      using PriorityTracerList   = ProductInventory<std::string, UIDType>;



      PsmrtsInvoice( ) : PsmrtsProduct( "PsmrtsInvoice" ),
                         PsmrtsRequest( "invoice_errors" ),
                         m_orders(  ),
                         m_processor( ),
                         m_inventory( "inventory" ),
                         m_priorities_t( create_case_sensitive_inventory<UIDType>("prioritytracers") ) { }
      PsmrtsInvoice( const std::string &name,
                     const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                     PsmrtsProduct( name ),
                     PsmrtsRequest( "InvoiceErrors" ),                     
                     m_orders( ),
                     m_processor( trans ),
                     m_inventory( name ),
                     m_priorities_t( create_case_sensitive_inventory<UIDType>("prioritytracers") ) { }                        
      virtual ~PsmrtsInvoice() { }
  

      inline size_t size() const {
        return ( m_orders.size() );
      }

      inline const std::string &name() const {
        return ( PsmrtsProduct::name() );
      }

      inline const PsmrtsTranslations &translations() const {
        return ( m_processor.translator() );
      }

      inline const ProductProcessing &processor() const {
        return ( m_processor );
      }

      inline const PsmrtsInventory &inventory() const {
        return ( m_inventory );
      }

      inline bool has_priority_tracer( const std::string & name ) {
        if ( !m_priorities_t.contains( name) ) return ( false );
        return ( m_inventory.prioritytracers().contains(m_priorities_t.find( name) ) );
      }
      
      inline PsmrtsPriorityTracer find_priority_tracer( const std::string & name = "" ) {
        return ( m_inventory.prioritytracers().find( m_priorities_t.find( name) ));
      }

      inline std::vector<std::string> get_priority_tracer_list(  ) {
        return ( m_priorities_t.keys() );
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
      
        // Add it to the invoice but don't generate products yet
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
      inline bool generate_products(  ) {
        // Reset the errors and regenerate the inventory
        m_processor.clear_errors();

        // Process the fdata inplace
        for ( auto &products : m_orders.data() ) {
          m_processor.process_product_set( products, m_inventory );
        }

        return ( m_processor.error_count() == 0  );
      }

      /**
       * @brief Get the priority tracer object generated from the product configs
       * 
       * This method will create a priority tracer from the results of the
       * configs contained in this invoice. Its possible the products have not
       * been generated yet so the generate_products() method should have been
       * called prior to calling this routine - it is renentrant.
       * 
       * If one is not found or products have not been generated yet.
       * 
       * @param name 
       * @return PsmrtsPriorityTracer 
       */
      inline PsmrtsPriorityTracer get_priority_tracer( const std::string &name = "" ) {

        std::string name_t = ( name.length() > 0 ) ? name : this->name();

        // First search the local inventory if one exists
        if ( this->has_priority_tracer( name_t ) ) {
          return ( this->find_priority_tracer( name_t )  );
        }

        // Check to see if we have any products. If not we assume
        // generate_products() has not been called so do it here.
        if ( m_inventory.size() == 0 ) {
          if ( !this->generate_products() ) {
            std::string mess = "PsmrtsInvoice::get_priority_tracer(" + name_t +
                              ") got errors generating products: " +
                              m_processor.errors_to_string( );
            this->add_error( mess );
            return ( PsmrtsPriorityTracer( "none" ));            
          }
        }

        PsmrtsPriorityTracer tracer_p( name_t );
        for ( const auto &tracer : m_inventory.tracers().cache() ) {
          tracer_p.add_tracer( tracer.second );
        }

        // Add to inventories and return the current tracer
        m_inventory.prioritytracers().add_product( tracer_p );
        m_priorities_t.add( name_t, tracer_p.uid() );

        return ( tracer_p );
      }

    private:
      ProductOrderList   m_orders;
      ProductProcessing  m_processor;
      PsmrtsInventory    m_inventory;
      PriorityTracerList m_priorities_t; 

  };

} // namespace psmrts

#endif
