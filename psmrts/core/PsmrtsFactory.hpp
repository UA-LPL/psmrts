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
#include <iostream>   
#include <sstream>  
#include <exception>
#include <map>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/products/PRQProduct.hpp>
#include <psmrts/core/products/ProductInventory.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>

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
   * This design also provides opportunities to create or copy existing
   * inventories for specialized/controlled environments. This will also
   * help aid in scaling management of PSMRTS environments.
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
   * "system" inventory called "psmrts". However, PsmrtsFactory provides
   * features to allow users to cache their own inventories for specialized
   * use. This feature allows users to create their own unique ray tracing
   * system while maintaining a complete copy of all its resources. This
   * By maintaining your own copy of your products, you will eliminate the
   * risk of other processings messing with your configured resources. You 
   * can uniquely name your products using unique characters as the names
   * of cached inventories are enforced lowercase names.
   * 
   * Note that PSMRTS is intented to provide a balance of stack and heap
   * resources. Efforts are made to use heap allocations for large data needs
   * and avoid instantiations of objects as pointers or even contain allocated
   * objects. Futhermore, use of virtual inheritance is discouraged with
   * efforts to support effective use of small objects within PSMRTS classes.
   * 
   * Here is a small example of the characteristics of PsmrtsFactory. This
   * example is a test case in our Catch2 test base that illustrates the
   * state of the PSMRTS factory product inventory environment in use.
   * 
   * @code
   * // State of an empty factory.
   * psmrts::PsmrtsFactory factory1;
   * factory1.liquidate();  // Start with empty factory, "psmrts" always exists
   *  
   * CHECK( factory1.size()               == 1 );
   * CHECK( factory1.contains( "psmrts" ) == true );
   * 
   * // Add a bullet tracer into unque inventory
   * std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
   * auto uid1 = factory1.add_product( psmrts::PsmrtsTracer::bullet( objfile ), "inv1" );
   * CHECK( factory1.size() == 2 );
   * CHECK( factory1.find( "inv1" ).tracers().size() == 1 );
   * CHECK( factory1.contains( "psmrts" )            == true );
   * CHECK( factory1.contains( "inv1" )              == true );
   * CHECK( factory1.contains( "INV1" )              == true );
   *
   * // Instantiate second factory and compare its state to factory1.
   * psmrts::PsmrtsFactory factory2;
   * CHECK( factory2.size()                          == 2 );
   * CHECK( factory2.size()                          == factory1.size() );
   * CHECK( factory2.contains( "psmrts" )            == factory1.contains( "psmrts" ) );
   * CHECK( factory2.contains( "inv1" )              == factory1.contains( "inv1" ) );
   * CHECK( factory2.find( "inv1" ).tracers().size() == factory1.find( "inv1" ).tracers().size() );
   *
   * @endcode 
   *
   * If your application requires an isolated ray tracing system, a custom
   * inventory can be built up through the contents of any number of
   * inventories. You can add (or merge) environments into one another,
   * or add to the system factory as a standalone inventory that can be
   * easily accessed from anywhere in your code simply by instantiating
   * a copy of PsmrtsFactory and find() the appropriate inventory - or
   * implement your own access capabilities.
   * 
   * @author Kris J Becker, Univerisity of Arizona
   * @history 2025-09-07 Kris J. Becker  Original Version
   * @history 2026-01-01 Kris J. Becker  Add thread locking for merge, add and
   *                      remove operations
   * @history 2026-02-17 Kris J. Becker  Fix initialization and tests
   */
  class PsmrtsFactory {
    public:
      inline static const std::string psmrts_inventory{ "psmrts" };
      using UIDType         = PsmrtsUID::UIDType;

      PsmrtsFactory( )  {  }
      virtual ~PsmrtsFactory() { }


      /** Return the number of inventories present in the factory */
      inline size_t size() const {
        std::scoped_lock mylocker( m_mutex );
        return ( PsmrtsFactory::inventory().size() );
      }

      /** Looking for an inventory by name case insensive */
      inline bool contains( const std::string &name ) const {
        std::scoped_lock mylocker( m_mutex );
        return ( PsmrtsFactory::inventory().contains( name ) );
      }

      /** Returns list of all cached inventories by name/uid */
      inline std::vector<std::string>  get_inventory_list( ) const {
        std::scoped_lock mylocker( m_mutex );
        return ( PsmrtsFactory::inventory().cache().keys() );
      }

      /** Looking for an inventory by name */
      inline const PsmrtsInventory &find( const std::string &name  = "psmrts" ) const {
        std::scoped_lock mylocker( m_mutex );
        return ( PsmrtsFactory::inventory().find( name ) );
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
      inline UIDType add_product( const PsmrtsShape &shape,
                                  const std::string &inventory_name = psmrts_inventory  ) {
                                    
        std::scoped_lock mylocker( m_mutex );
        if ( PsmrtsFactory::inventory().contains( inventory_name ) ) {
          return ( PsmrtsFactory::inventory().find( inventory_name ).shapes().add_product( shape ) );
        }
        else {
          PsmrtsInventory inv_t( inventory_name );
          auto uid = inv_t.shapes().add_product( shape );
          PsmrtsFactory::inventory().add( inventory_name, inv_t );
        }           
        return ( shape.uid() );
      }

      /** Remove a shape product from a named inventory */
      inline void remove_shape( const UIDType &uid, 
                                const std::string &inventory_name = psmrts_inventory  ) {
        std::scoped_lock mylocker( m_mutex );
        if ( PsmrtsFactory::inventory().contains( inventory_name )  ) {
          PsmrtsFactory::inventory().find( inventory_name ).shapes().remove( uid );
        }
        return;
      } 


      /**
       * @brief Add a PsmrtsTracer to the named inventory
       * 
       * This method will add a PsmrtsTracer to a named inventory in the
       * PSMRTS factory system. If the specified inventory does not exist,
       * it will be created and the product added to the inventory.
       * 
       * @param tracer         PsmrtsTracer to add to the named inventory
       * @param inventory_name Name of inventory to add the tracer to [default: "psmrts"]
       * @return UIDType       The unique id of the product as stored in the cache
       */
      inline UIDType add_product( const PsmrtsTracer &tracer,
                                  const std::string &inventory_name = psmrts_inventory  ) {
        std::scoped_lock mylocker( m_mutex );
        if ( PsmrtsFactory::inventory().contains( inventory_name )  ) {
          return ( PsmrtsFactory::inventory().find( inventory_name ).tracers().add_product( tracer ) );
        }
        else {
          PsmrtsInventory inv_t( inventory_name );
          auto uid = inv_t.tracers().add_product( tracer );
          PsmrtsFactory::inventory().add( inventory_name, inv_t );
        }        
        return ( tracer.uid() );
      }

      /** Remove a tracer from a named inventory */
      inline void remove_tracer( const UIDType &uid, 
                                const std::string &inventory_name = psmrts_inventory  ) {
        std::scoped_lock mylocker( m_mutex );
        if ( PsmrtsFactory::inventory().contains( inventory_name )  ) {
          PsmrtsFactory::inventory().find( inventory_name ).tracers().remove( uid );
        }
        return;
      }       

      /**
       * @brief Add a PsmrtsPriorityTracer to the named inventory
       * 
       * This method will add a PsmrtsPriorityTracer to a named inventory in
       * the PSMRTS factory system. If the specified inventory does not exist,
       * it will be created and the product added to the inventory.
       * 
       * @param tracer_p         PsmrtsPriorityTracer to add to the named inventory
       * @param inventory_name Name of inventory to add the tracer to [default: "psmrts"]
       * @return UIDType       The unique id of the product as stored in the cache
       */
      inline UIDType add_product( const PsmrtsPriorityTracer &tracer_p,
                                  const std::string &inventory_name = psmrts_inventory  ) {
        std::scoped_lock mylocker( m_mutex );
        if ( PsmrtsFactory::inventory().contains( inventory_name )  ) {
          return ( PsmrtsFactory::inventory().find( inventory_name ).prioritytracers().add_product( tracer_p ) );
        }
        else {
          PsmrtsInventory inv_t( inventory_name );
          auto uid = inv_t.prioritytracers().add_product( tracer_p );
          PsmrtsFactory::inventory().add( inventory_name, inv_t );
        }
        return ( tracer_p.uid() );
      }

      /** Remove a priority tracer from a named inventory */
      inline void remove_priority_tracer( const UIDType &uid, 
                                          const std::string &inventory_name = psmrts_inventory  ) {
        std::scoped_lock mylocker( m_mutex );
        if ( PsmrtsFactory::inventory().contains( inventory_name )  ) {
          PsmrtsFactory::inventory().find( inventory_name ).prioritytracers().remove( uid );
        }
        return;
      }                                            

      /** Add a value to the cache - overwrites existing data */
      inline size_t add( const PsmrtsInventory &inventory, 
                       const std::string &cache_name = psmrts_inventory) {
        return ( this->merge( inventory, cache_name ) );
      }

      /** Get the current state of the parameter/environment variable system */
      static inline PsmrtsTranslations getenv( ) {
        return ( PsmrtsTranslations::create() );
      }

      
      /** Remove the requested cache value by key */
      inline size_t merge( const PsmrtsInventory &inventory, 
                           const std::string &cache_name ) {

        std::scoped_lock mylocker( m_mutex );
        size_t n_merged = 0;

        if ( PsmrtsFactory::inventory().contains( cache_name ) ) {
          PsmrtsInventory &cache = PsmrtsFactory::inventory().find( cache_name );
          n_merged += cache.merge( inventory );
        }
        else {
          PsmrtsFactory::inventory().add( cache_name, inventory );
          n_merged += inventory.size();
        }

        return ( n_merged );
      }

      /** Remove the requested cache value by key */
      inline size_t merge( const PsmrtsInventory &inventory ) {
        return ( this->merge( inventory, psmrts_inventory ) );
      }

      /** Remove a system inventory from the factory! */
      inline void remove( const std::string &invname ) {
        std::scoped_lock mylocker( m_mutex );
        PsmrtsFactory::inventory().remove( invname );
      }
    
      /** Liquidate/empty all PSRMTS factory inventory - affects all instances of PsmrtsFactory! */
      inline static void liquidate( ) {
        std::scoped_lock mylocker( m_mutex );
        PsmrtsFactory::PsmrtsFactory::inventory().clear();
        
        // Be sure to set up the default "psmrts" inventory
        PsmrtsFactory::PsmrtsFactory::inventory().add( "psmrts", PsmrtsInventory("psmrts") );
        return;
      }

    private:
      // Definitions and cache of active product inventories.
      using FactoryInventory = ProductInventory<std::string, PsmrtsInventory>;
      static inline std::mutex       m_mutex{};

      // Flag to initialize at startup
      static inline std::once_flag psmrts_inventory_init;

      /** Return the factory inventory */
      static inline FactoryInventory &inventory()  {
        static FactoryInventory m_inventory;
         std::call_once( psmrts_inventory_init, [&]( ){ 
            m_inventory = create_case_insensitive_inventory<PsmrtsInventory>( "psmrts" );
            m_inventory.add( "psmrts", PsmrtsInventory("psmrts") ); 
          } ); // set up default product inventory cache on first call

        return ( m_inventory );
      }
  };

} // namespace psmrts

#endif
