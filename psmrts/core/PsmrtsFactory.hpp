/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsFactory_hpp
#define PsmrtsFactory_hpp

#include <string>
#include <functional>
#include <iostream>   
#include <iterator>
#include <sstream>  
#include <exception>
#include <tuple>
#include <map>
#include <shared_mutex>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsSharedCache.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>

namespace psmrts {

  /**
   * @brief PSMRTS Factory provider of all products
   * 
   * This class contains a persistent inventory of all PSMRTS products.
   * Inventories can be created for most any situation and copied
   * freely as standalone resources. Using this class for all PSMRTS
   * product quieries will centralize the caching of all products during
   * systematic processing. 
   * 
   * PsmrtsFactory behaves very much like a singleton class object with
   * the main difference being it does not use a pointer to reference
   * the class methods and data. Instead, you can create an instance of
   * this class where each resulting instance contains the same exact
   * data. With any instance of PsmrtsFactory, you can alter the contents
   * of the internal inventory cache for all PSMRTS products. You can
   * even completety empty the cache which clearly impacts all users
   * of the factory.
   * 
   * If you need an enviroment that does not have this type of dynamic, then
   * PsmrtsInventory is what you need. See PsmrtsInventory.hpp for details
   * of its characteristics. 
   * 
   * PsrmtsFactory contains a cache of PsmrtsInventorys with the main default
   * 
   * Here is a small example of the characteristics of PsmrtsFactory.
   * 
   * @code
   * // State of an empty factory.
   * psmrts::PsmrtsFactory factory1;
   * factory1.liquidate();  // Start with empty factory, "psmrts" always exists
   *  
   * @endcode 
   *
   * @author Kris J Becker, Univerisity of Arizona
   * @history 2025-09-07 Kris J. Becker  Original Version
   * @history 2026-01-01 Kris J. Becker  Add thread locking for merge, add and
   *                      remove operations
   * @history 2026-02-17 Kris J. Becker  Fix initialization and tests
   */
  class PsmrtsFactory final {
    public:
      using UIDType                 = PsmrtsUID::UIDType;
      using TracerInventory         = PsmrtsInventory::TracerInventory;
      using ShapeInventory          = PsmrtsInventory::ShapeInventory;
      using SharedTracerInventory   = PsmrtsInventory::SharedTracerInventory;
      using SharedShapeInventory    = PsmrtsInventory::SharedShapeInventory;      
      using ParameterInventory      = PsmrtsInventory::ParameterInventory;

      PsmrtsFactory( ) = default;
      ~PsmrtsFactory() = default;

      //*** Shape utilities ***//
      
      /** Return the number of shapes in the inventory */
      inline size_t shape_count() const {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.shapes()->size() );
      }

      /**
       * @brief Add PsmrtsShape object into a named inventory
       * 
       * This method will add a PsmrtsShape to a named inventory in the
       * PSMRTS factory system. If the specified inventory does not exist,
       * it will be created and the product added to the inventory.
       * 
       * @param shape          PsmrtsShape to add to named inventory
       * @param inventory_name Name of the inventory to add the shape to
       * @return UIDType       The unique id of the product as stored in the cache
       */
      inline UIDType add( const PsmrtsShape &shape  ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.add( make_shared_copy( shape ) ) );
      }

      inline UIDType add( const SharedShape &shape  ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        return (  m_inventory.add( shape ) );
      }

      inline bool contains_shape( const UIDType &uid  ) {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.shapes()->contains( uid ) );
      }

      inline SharedShape find_shape( const UIDType &uid  ) {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.shapes()->find( uid ) );
      }

      /** Remove a shape product from a named inventory */
      inline void remove_shape( const UIDType &uid  ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        m_inventory.shapes()->remove( uid );
        return;
      } 

      inline std::vector<UIDType> shape_keys() const {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.shapes()->keys() );
      }

      //*** Tracer utilities ***//

      /** Return the number of tracers in the inventory */
      inline size_t tracer_count() const {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.tracers()->size() );
      }

      /**
       * @brief Add a PsmrtsTracer to the inventory
       * 
       * This method will add a PsmrtsTracer to a named inventory in the
       * PSMRTS factory system. If the specified inventory does not exist,
       * it will be created and the product added to the inventory.
       * 
       * @param tracer         PsmrtsTracer to add to the named inventory
       * @param inventory_name Name of inventory to add the tracer to [default: "psmrts"]
       * @return UIDType       The unique id of the product as stored in the cache
       */
      inline UIDType add( const PsmrtsTracer &tracer ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.add( make_shared_copy( tracer ) ) );
      }

      inline UIDType add( const SharedTracer &tracer ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.add( tracer ) );
      }

      inline bool contains_tracer( const UIDType &uid  ) {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.tracers()->contains( uid ) );
      }

      inline SharedTracer find_tracer( const UIDType &uid  ) {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.tracers()->find( uid ) );
      }

      /** Remove a tracer from a named inventory */
      inline void remove_tracer( const UIDType &uid  ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        m_inventory.tracers()->remove( uid );
        return;
      }     
      
      inline std::vector<UIDType> tracer_keys() const {
        std::shared_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.tracers()->keys() );
      }

      /** Add a value to the cache - overwrites existing data */
      inline size_t add( const PsmrtsInventory &inventory ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.merge( inventory ) );
      }      
      
      /** Merge the requested cache into the factory */
      inline size_t merge( const PsmrtsInventory &inventory ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        return ( m_inventory.merge( inventory ) );
      }

      /**
       * @brief Comprehensive product maker complete with search of existing resources
       * 
       * This method will search the factory inventory for a specific product
       * given the configurations present. Note this method will search the
       * "inventory" contents and then factory resources for existing
       * products that may exist.
       * 
       * It is assumed that the products contained in the order are validated
       * against a product specification.
       * 
       * If the product does not exist, it will be created and added to the 
       * factory resources as well as the "inventory" parameter.
       * 
       * There are no more that two products expected to exist in the order - 
       * a tracer and shape.
       * 
       * The errors parameter will inform the caller of any errors that have
       * occurred.
       * 
       * @param orders    Array of orders, typically from an invoice
       * @param inventory The local inventory to check for tracer products
       * @return std::vector<SharedTracer> Ordered list of tracers created so
       *                     that priority is preserved if that is needed by
       *                      called
       */
      inline std::vector<SharedTracer> process_order( const std::vector<SharedOrder> &orders,
                                                      PsmrtsInventory &inventory,
                                                      PsmrtsErrors &errors ) {

        std::unique_lock<std::shared_mutex> mylocker( m_mutex );

        std::vector<SharedTracer> tracer_list_t;
        tracer_list_t.reserve( orders.size() );

        ProductProcessing processor( inventory.translations() );
        for ( const auto &order : orders ) {

          // Refuse to process an invalid empty product
          if ( !order->isvalid() ) {
            errors.add_error( "PsmrtsFactory::make_product() - Invalid product order: " + order->name() );
            return ( tracer_list_t );
          }

          // Get the cart configuration
          SharedCart cart_t = order->find( "tracer" );
          SharedCart cart_s = order->find( "shape" );

          SharedTracer tracer_p;
          SharedShape  shape_p;

          // Run a search on the local inventory since we can be creating new
          // products there.
          bool success = this->tracer_search( cart_t, cart_s, 
                                              { inventory.tracers(), 
                                                m_inventory.tracers() },
                                              processor, tracer_p, shape_p );
          
          // If we did not find an existing tracer, we must make one
          if ( !success ) {

            // First see if we have a shape
            this->shape_search( cart_s, 
                                { inventory.shapes(), m_inventory.shapes() },
                                processor, shape_p );

            if ( !this->make_tracer_and_shape( *order, tracer_p, shape_p, errors) ) {
              errors.throw_errors();
            }

          }

          this->update_cart( cart_t, tracer_p );  
          this->update_cart( cart_s, shape_p ); 
        
          // Update the invertories
          inventory.add( tracer_p );
          inventory.add( shape_p );


          m_inventory.add( tracer_p );
          m_inventory.add( shape_p );

          tracer_list_t.push_back( tracer_p );
        }
        return ( tracer_list_t );
      }  
      
      /** Create tracers from a single order */
      inline std::vector<SharedTracer> process_order( SharedOrder &order,
                                                     PsmrtsInventory &inventory,
                                                     PsmrtsErrors &errors ) {
        std::vector<SharedOrder> orders =  { order };
        return ( process_order( orders, inventory, errors ) );
      }

      /**
       * @brief Returns a reference to the translator containing users evironment
       * 
       * This method returns a reference to the current set of environment
       * variables in the user's shell. This is intended to provide a consistent
       * starting point where users can copy this and add parameters as needed
       * to assist in translating file paths.
       * 
       * This is inspired by the ISIS IsisPreferences system that defines a set
       * of environment-like variables that define an absolute path to mission
       * ancillary data such as NAIF SPICE kernels and calibration data. This
       * system uses both parameters and environment variables that are
       * iteratively substituted to provide absolute file paths in file names.
       * 
       * See ISIS classes Preferences, FileName and the IsisPreferences file in
       * $ISISROOT.
       * 
       * @see PsmrtsTranslations.hpp.
       * 
       * @return const PsmrtsTranslations& Current state of users environment
       *                                     variables
       */
      inline const PsmrtsTranslations &translator() const {
        return ( *m_inventory.translations() );
      }
      
      /** Get the current state of the parameter/environment variable system */
      static inline PsmrtsTranslations getenv( ) {
        return ( PsmrtsTranslations::create() );
      }

      /** Liquidate/empty all PSRMTS factory inventory - affects all instances of PsmrtsFactory! */
      inline static void liquidate( ) {
        std::unique_lock<std::shared_mutex> mylocker( m_mutex );
        m_inventory.clear();
        return;
      }

    private:
      // Definitions and cache of active product inventories.
      // using FactoryInventory = InventoryCache;
      static inline PsmrtsInventory m_inventory{ "psmrts" };
      static inline std::shared_mutex  m_mutex{};

      template <typename SharedT>
        inline void update_cart( SharedCart &cart, const SharedT &data ) const {
          if ( cart && data ) cart->set_uid( data->uid() );
        }

      inline bool tracer_search( const SharedCart &cart_t,
                                 const SharedCart &cart_sptr, 
                                 const std::initializer_list<SharedTracerInventory> &tracer_list,
                                 const ProductProcessing &processor,
                                 SharedTracer &tracer,
                                 SharedShape &shape ) const {

        if ( !cart_t ) return ( false );

        for ( const auto &tracer_inv : tracer_list ) {
          auto tracer_p = processor.search_tracer_inventory( *cart_t, *tracer_inv );
          if ( tracer_p ) {
            // Got a tracer so check shape conditions
            if ( cart_sptr ) {
              PRQShape shape_extractor;
              if ( tracer_p->process( shape_extractor ) ) {

                // Check if the cart and shape specs are compatible
                auto shape_p = shape_extractor.shape();
                if ( cart_sptr->specification().name() == shape_p->specs().name() ) {

                  // Compare the tracer's shape specs with the shape cart
                  ProductCart cart_shape( shape_p->specs(), shape_p->config() );
                  PsmrtsErrors errors;
                  if ( processor.compare_product_config( cart_sptr->configuration(), cart_shape, errors ) ) {
                    tracer = tracer_p;
                    shape  = shape_p;
                    return ( true );
                  }
                }
              }
            }
            else {
              tracer = tracer_p;
              return ( tracer.get() != nullptr );
            }
          }
       }

        return ( false );
      } 
      
      inline bool shape_search( const SharedCart &cart_s, 
                                const std::initializer_list<SharedShapeInventory> &shape_list,
                                const ProductProcessing &processor,
                                SharedShape &shape ) const {

        if ( !cart_s ) return ( false );
        for ( const auto &list_s : shape_list ) {
          auto shape_p = processor.search_shape_inventory( *cart_s, *list_s );
          if ( shape_p ) {
            shape = shape_p;
            return ( true );
          }
        }
        return ( false );
      } 

      inline bool make_tracer_and_shape( const ProductOrder &order,
                                         SharedTracer &tracer, 
                                         SharedShape &shape,
                                         PsmrtsErrors &errors ) const {

        SharedCart cart_s = order.find("shape");
        SharedCart cart_t = order.find("tracer");

        bool success = true;
        if ( cart_s && !shape ) {
          ProductMaker<PsmrtsShape> maker_s( cart_s->name() );
          if ( !maker_s.process_cart( *cart_s ) ) {
            errors.add_error( "PsmrtsFactory::make_tracer_and_shape() - Failed to make shape: " + cart_s->name() );
            return ( false );
          }
          shape = maker_s.product();     
        }

        if ( cart_t && !tracer ) {
          ProductMaker<PsmrtsTracer> maker_t( cart_t->name() );
          SharedTracer tracer_t;
          if ( !shape ) {
            tracer_t = maker_t.process_cart( *cart_t );
          }
          else {
            tracer_t = maker_t.process_cart( *cart_t, shape );
          }

          // Check for success
          if ( !tracer_t ) {
            errors.add_error( "PsmrtsFactory::make_tracer_and_shape() - Failed to make tracer: " + cart_t->name() );
            success = false;
          }
          else {
            tracer = tracer_t;
          }

        } 
        return ( success );
      } 
      
  };

} // namespace psmrts

#endif
