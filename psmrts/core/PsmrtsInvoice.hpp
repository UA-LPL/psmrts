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
#include <psmrts/algorithms/VariantTraits.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/products/ProductInventory.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
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

      inline bool has_priority_tracer( const std::string & name ) const {
        if ( !m_priorities_t.contains( name) ) return ( false );
        return ( m_inventory.prioritytracers().contains(m_priorities_t.find( name) ) );
      }
      
      inline PsmrtsPriorityTracer find_priority_tracer( const std::string & name = "" ) {
        return ( m_inventory.prioritytracers().find( m_priorities_t.find( name) ));
      }

      inline std::vector<std::string> get_priority_tracer_list() const {
        return ( m_priorities_t.keys() );
      }

      /**
       * @brief Process a product configuration, create product set (shape, tracer)
       * 
       * This method processes a product configuration and creates a product set
       * that may contain a shape and/or a tracer. 
       * 
       * @param config      Product configuration
       * @return ProductSet A product containing a shape/tracer
       */
      inline ProductSet process_product( const ProductConfiguration &config ) {

        // Reset processor errors stack
        m_processor.clear_errors();

        // Parse/evalute the configuration
        ProductSet product_s = m_processor.process_configuration( config );
        if ( !m_processor.is_valid_product( product_s ) ) {
          std::string mess = "PsmsrtsInvoice::process_product() - " + 
                              config.name() +
                              ") config validation errors: " +
                              m_processor.product_error_string( product_s );
          this->add_error( mess );
          return ( product_s );
        }

        // Process the product set
        if ( !m_processor.process_product_set( product_s, m_inventory ) ) {
          std::string mess = "PsmrtsInvoice::process_product() - " + 
                             config.name() +
                              ") product creation errors: " +
                              m_processor.product_error_string( product_s );
          this->add_error( mess );
          return ( product_s );          
        }
        
        // User can evaluate product set status
        return ( product_s );
      }


      /**
       * @brief Adds a product set to the internal container
       * 
       * This method adds a processed product set to the internal product set
       * array. The product set typically should be error free and contain at
       * least a shape and/or a tracer. It will not be added if the these
       * conditions are not met.
       * 
       * @param product_set A processed product set containing a valid product
       * @return true  If the product set is valid 
       * @return false If the product set does not contain a valid product
       */
      inline bool add_product( const ProductSet &product_set ) {

        // Must have at least one valid product - shape or tracer
        if ( !m_processor.has_valid_product( product_set ) ) {
          return ( false );
        }
        
        // Add the valid product
        m_orders.add( product_set );
        return (true );
      }

      /**
       * @brief Creates and adds a product to the invoice from a config
       * 
       * @param config Product configuration to create valid product
       * @return true  If creation was successfull
       * @return false If product could not be created
       */
      inline bool create_product( const ProductConfiguration &config ) {
        return ( add_product( process_product( config ) ) );
      }

      /**
       * @brief Add a PSMRTS tracer to the inventory and tracer list
       * 
       * Use this method to directly add a existing PSMRTS tracer to the
       * tracing system. It will be added to the configuration of the priority
       * tracer and to the inventory. If the tracer already exists in the
       * system, it will not replace its instance in the inventory, but will be
       * added to the priority tracer configuration for future use.
       * 
       * @param tracer Tracer to add to the inventory/priority tracer system
       * @return true  If the tracer is valid and the config is validated for use
       * @return false If the tracer was not added to the system
       */
      inline bool add_tracer( const PsmrtsTracer &tracer,
                              const bool add_to_priority = true ) {

        bool isgood = tracer.isValid();
        if ( tracer.isValid() ) {
          m_inventory.tracers().add_product ( tracer );
          PsmrtsFactory().add_product( tracer );
        }

        return ( isgood );
      }

      /**
       * @brief Add a PSMRTS shape to the inventory
       * 
       * Use this method to directly add a existing PSMRTS shape to the
       * inventory system. If the shape already exists in the system, it will
       * not replace its instance in the inventory, but will be made available
       * for use in tracer configurations.
       * 
       * @param tracer Shape to add to the inventory/priority inventory system
       * @return true  If the shape is valid and added to the inventory system
       * @return false If the shape was not added to the system
       */
      inline bool add_shape( const PsmrtsShape &shape ) {

        bool isgood = shape.isValid();
        if ( shape.isValid() ) {
          if ( isgood ){
            m_inventory.shapes().add_product ( shape );
            PsmrtsFactory().add_product( shape );
          }
        }

        return ( isgood );
      }      

      /** Get a list of all the orders in this invoice */
      inline const ProductOrderList &orders( ) const {
        return ( m_orders );
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

        PsmrtsPriorityTracer tracer_p( name_t );
        for ( const auto &product : this->orders() ) {
          if ( product.has_tracer() ) {
            tracer_p.add_tracer( product.tracer_p.value() );
          }
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
