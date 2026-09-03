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
   * Note that duplicate tracers are prevented from being in the priority tracer
   * so the total count of tracers in the prioriy tracer may not be the same 
   * as the number of sumbitted orders. Duplicate tracers will cause extended
   * run times for traces that will not produce different results from previous
   * traces in the priority tracer, so they are excluded.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   * @history 2026-08-26 Kris J. Becker Refactored to use heap memory
   */
  class PsmrtsInvoice : public PsmrtsErrors {
    public:
      using UIDType          = PsmrtsProduct::UIDType;
      using ProductOrderList = std::vector<SharedOrder>;
      using TracerList       = std::vector<SharedTracer>;

      PsmrtsInvoice( ) : PsmrtsErrors(),
                         m_name( "invoice" ),
                         m_orders(  ),
                         m_inventory( make_shared_copy( PsmrtsInventory( ) ) ),
                         m_tracers() { }
      PsmrtsInvoice( const std::string &name,
                     const PsmrtsTranslations &trans = PsmrtsTranslations() ) : 
                     m_name( name ),
                     m_orders( ),
                     m_inventory( ),
                     m_tracers() {
        m_inventory = make_shared_copy( PsmrtsInventory( name, trans ) );    
      }
      PsmrtsInvoice( const std::string &name,
                     const SharedTranslations &trans) : 
                     m_name( name ),
                     m_orders( ),
                     m_inventory( ),
                     m_tracers() {
        m_inventory = make_shared_copy( PsmrtsInventory( name, trans ) );    
      }

      virtual ~PsmrtsInvoice() = default;
  
      inline const std::string &name() const {
        return ( m_name );
      }

      /** Returns the size of the orders */
      inline size_t size() const {
        return ( m_orders.size() );
      }

      /**
       * @brief Checks the state of the orders and the tracer list for consistency
       * 
       * This method detects if the order size is consistent with the tracer list.
       * If they are not the same, the submit_order() method should be run.
       * 
       * @return true   If the sizes of the orders and tracers are the same
       * @return false  If they are not the same indicating a submit should be
       *                  ran
       */
      inline bool isvalid() const {
        return ( m_tracers.size() == m_orders.size() );
      }

      /** Returns the file path translator  */
      inline const PsmrtsTranslations &translations() const {
        return ( *m_inventory->translations() );
      }

      /** Returns the inventory for this invoice after submit_order() */
      inline const PsmrtsInventory &inventory() const {
        return ( *m_inventory );
      }

      /**
       * @brief Process a product configuration, create product set (shape, tracer)
       * 
       * This method processes a product configuration and adds a product order
       * to the invoice list.
       * 
       * This method does not create the tracer. See submit_order().
       * 
       * @param config Product configuration for the tracer
       */
      inline void add( const ProductConfiguration &config ) {

        // Reset processor errors stack
        this->clear_errors();
        ProductProcessing processor( m_inventory->translations() );
        auto order = processor.process_order( config );
        if ( this->error_count() > 0 ) {
          this->add_error( "PsmrtsInvoice::add() - Failed to process config order " + config.name() );
          this->throw_errors();
        }

        // All good
        m_orders.push_back( order );
        return;
      }

      /**
       * @brief Submit the list of orders to factory for processing
       * 
       * The list of orders are submitted to the factory for processing. This
       * will result in a local inventory of all the tracers and shapes generated
       * in the list of processed configurations.
       * 
       * Note this method is reetrant. Additional configurations can be added 
       * and submitted as the factory will check the local inventory for products
       * first and then its resources. A new priority tracer can then be generated
       * at any time. If the number of tracers do not match the number of orders
       * it is sent back the factory for an update.
       * 
       * @return size_t Total number of tracers created from list of orders
       */
      inline size_t submit_order() {
        m_tracers = PsmrtsFactory().process_order( m_orders, *m_inventory, *this );
        if ( m_tracers.size() != m_orders.size() ) {
          // this is an erorr
          this->add_error( "PsmrtsInvoice::submit_order() - Number tracers returned (" +
                           std::to_string( m_tracers.size() ) + ") not equal to orders (" +
                           std::to_string( m_orders.size() ) + ")" );
        }
        if ( this->has_errors() ) this->throw_errors();
        return ( m_tracers.size() );
      }

      /**
       * @brief Create list of PsmrtsTracers without duplicate tracers
       * 
       * User tracer configurations at times can result duplications of tracers.
       * This will result in longer trace times as repeat traces on the same
       * tracer will produce the same result. This method removes duplicate
       * tracers produced after processing by submit_order().
       * 
       * BEWARE this method may return less tracers than the tracers() method!!
       * The result of the tracers() method may contain duplicate tracers. This 
       * method removes duplicate tracers by uids.
       * 
       * @return TracerList Optimized list of tracers without duplicates
       */
      inline TracerList optimized_tracer_list() const {
        return ( remove_duplicates( m_tracers ) );
      }

      /**
       * @brief Get the priority tracer object generated from the product configs
       * 
       * This method will create a priority tracer from the results of the
       * configs/orders contained in this invoice. It will generate a priority tracer
       * from the contents of the order list and can be called any number
       * of times - even after adding new products.
       * 
       * A new instance of a priority tracer is returned since order is maintained
       * in each priority tracer. 
       * 
       * Note that the number of tracers in the priority tracer may not match
       * the number of orders in the invoice. Redundent tracers are removed
       * before the priority tracer is created. Each priority tracer is unique
       * and does not share the same list (but tracer instances are shared).
       * It would be quite inefficient and unnecessary to have duplicate tracers
       * in the priorty tracer.
       * 
       * @param name  Name of the priority tracer to create.
       * @return PsmrtsPriorityTracer A priority tracer from the order set
       */
      inline PsmrtsPriorityTracer make_priority_tracer( const std::string &name = "" ) {
        if ( !this->isvalid() ) this->submit_order();
        std::string name_t = ( name.length() > 0 ) ? name : m_name;
        return ( PsmrtsPriorityTracer( name_t, optimized_tracer_list() ) );
      }

      /** Returns a list of tracers in the order (priority) submitted */
      inline const std::vector<SharedTracer> &tracers() const {
        return ( m_tracers );
      }

      /** Returns a list of processed order configurations in the order (priority) submitted */
      inline const ProductOrderList &orders() const {
        return ( m_orders );
      }      

      /** Construct and return a product processor consistent with this object */
      inline ProductProcessing processor() const {
        return ( ProductProcessing( m_inventory->translations() ) );
      }

    private:
      std::string      m_name;
      ProductOrderList m_orders;
      SharedInventory  m_inventory;
      TracerList       m_tracers;

      /**
       * @brief Removes redundant tracers from the list after submission
       * 
       * This method will check tracers after they are created to determine if
       * they have already been added to the list. Under most circumstances, 
       * there is no reason to have redundant tracers in the a priority tracer
       * as it simply adds overhead. This because the first result of a trace
       * will be the same in duplicates of other tracers. 
       * 
       * The determination of redundant tracers is made by checking for the
       * same tracer pointers. 
       * 
       * @param  list_t
       * @return TracerList List of unique tracers in ist_t
       */
      inline TracerList remove_duplicates( const TracerList &list_t ) const {
        TracerList newlist_t;
        newlist_t.reserve( list_t.size() );

        /** Checks for duplicate except at index_t assumed to be t */
        auto tracer_dup = [&]( auto &t, const size_t max_i ) -> bool {
          for ( size_t j = 0 ; j < max_i ; j++ ) {
            if ( t->uid() == list_t[j]->uid() ) return ( true );
          }
          return ( false );
        };

        // Check for duplicates and only return unique ones
        for ( size_t i = 0 ; i < list_t.size() ; i++ ) {
          if ( !tracer_dup( list_t[i], i ) ) {
            newlist_t.push_back( list_t[i] );
          }
        }

        return ( newlist_t );
      }

  };

  // Declare a shared pointer type for tracers
  using SharedInvoice = std::shared_ptr<PsmrtsInvoice>;

} // namespace psmrts

#endif
