#ifndef PsmrtsFactory_hpp
#define PsmrtsFactory_hpp

#include <string>
#include <iostream>   
#include <sstream>  
#include <exception>
#include <map>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PRQProduct.hpp>
#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/PRQProduct.hpp>

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
   *  // State of an empty factory.
   *  psmrts::PsmrtsFactory factory1;
   *  CHECK( factory1.size()               == 0 );
   *  CHECK( factory1.contains( "psmrts" ) == false );
   *
   *  // Add a bullet tracer into unque inventory
   *  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
   *  auto uid1 = factory1.add_product( psmrts::PsmrtsTracer::bullet( objfile ), "inv1" );
   *  CHECK( factory1.size() == 1 );
   *  CHECK( factory1.find( "inv1" ).tracers().size() == 1 );
   *  CHECK( factory1.contains( "psmrts" )            == false );
   *  CHECK( factory1.contains( "inv1" )              == true );
   *  CHECK( factory1.contains( "INV1" )              == true );
   * 
   *  // Instantiate second factory and compare its state to factory1.
   *  psmrts::PsmrtsFactory factory2;
   *  CHECK( factory2.size()                          == 1 );
   *  CHECK( factory2.size()                          == factory1.size() );
   *  CHECK( factory2.contains( "psmrts" )            == factory1.contains( "psmrts" ) );
   *  CHECK( factory2.contains( "inv1" )              == factory1.contains( "inv1" ) );
   *  CHECK( factory2.find( "inv1" ).tracers().size() == factory1.find( "inv1" ).tracers().size() );
   * 
   *  // Adding a new shape to a new inventory will appear in both (all) factory objects.
   *  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
   *  psmrts::DskShape dsk( dskfile );
   *  auto uid2 = factory2.add_product( psmrts::PsmrtsShape( dsk ), "inv2");
   *  CHECK( factory2.size()                           == 2 );
   *  CHECK( factory2.size()                          == factory1.size() );
   *  CHECK( factory2.contains( "inv2" )              == true );
   *  CHECK( factory2.contains( "inv2" )              == factory1.contains( "inv2" ) );
   *  CHECK( factory2.find( "inv2" ).shapes().size()  == 1 );
   *  CHECK( factory2.find( "inv2" ).shapes().size()  == factory1.find( "inv2" ).shapes().size());
   * 
   *  // PSMRTS inventory cache content check
   *  CHECK( factory1.get_inventory_list() == std::vector<std::string>( { "inv1", "inv2" } ) );
   *  CHECK( factory2.get_inventory_list() == factory1.get_inventory_list() );
   * 
   *  // Removing a product in one factory affects all factory instances
   *  CHECK( factory2.find( "inv2" ).shapes().contains( uid2 ) == true );
   *  CHECK( factory1.find( "inv2" ).shapes().contains( uid2 ) == true ); 
   *  CHECK_NOTHROW( factory2.find( "inv2" ).shapes().remove( uid2 ) );
   * 
   *  CHECK( factory2.find( "inv2" ).shapes().contains( uid2 ) == false );
   *  CHECK( factory1.find( "inv2" ).shapes().contains( uid2 ) == false );
   *  CHECK( factory2.find( "inv2" ).shapes().size()           == 0 );
   *  CHECK( factory2.find( "inv2" ).shapes().size()           == factory1.find( "inv2" ).shapes().size() );
   *
   *  CHECK( factory1.size()   == 2 );
   *  CHECK( factory2.size()   == 2 );
   *  factory1.liquidate();
   *  CHECK( factory1.size()               == 0 );
   *  CHECK( factory2.size()               == 0 );
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
   */
  class PsmrtsFactory {
    public:
      inline static const std::string psmrts_inventory{ "psmrts" };
      using UIDType         = PsmrtsUID::UIDType;

      PsmrtsFactory( )  {  }
      virtual ~PsmrtsFactory() { }


      /** Return the number of inventories present in the factory */
      inline size_t size() const {
        return ( this->inventory().size() );
      }

      /** Looking for an inventory by name case insensive */
      inline bool contains( const std::string &name ) const {
        return (this->inventory().contains( name ) );
      }

      /** Returns list of all cached inventories by name/uid */
      inline std::vector<std::string>  get_inventory_list( ) const {
        return ( m_inventory.cache().keys() );
      }      

      /** Looking for an inventory by name */
      inline PsmrtsInventory &find( const std::string &name ) {
        return (this->inventory().find( name ) );
      }

      /** Looking for an inventory by name */
      inline const PsmrtsInventory &find( const std::string &name ) const {
        return (this->inventory().find( name ) );
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
        if ( this->contains( inventory_name ) ) {
          return ( this->inventory().find( inventory_name ).shapes().add_product( shape ) );
        }
        else {
          PsmrtsInventory inv_t( inventory_name );
          auto uid = inv_t.shapes().add_product( shape );
          this->inventory().add( inventory_name, inv_t );
        }           
        return ( shape.uid() );
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
        if ( this->contains( inventory_name ) ) {
          return ( this->inventory().find( inventory_name ).tracers().add_product( tracer ) );
        }
        else {
          PsmrtsInventory inv_t( inventory_name );
          auto uid = inv_t.tracers().add_product( tracer );
          this->inventory().add( inventory_name, inv_t );
        }        
        return ( tracer.uid() );
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
        if ( this->contains( inventory_name ) ) {
          return ( this->inventory().find( inventory_name ).prioritytracers().add_product( tracer_p ) );
        }
        else {
          PsmrtsInventory inv_t( inventory_name );
          auto uid = inv_t.prioritytracers().add_product( tracer_p );
          this->inventory().add( inventory_name, inv_t );
        }
        return ( tracer_p.uid() );
      }


      /** Add a value to the cache - overwrites existing data */
      inline size_t add( const PsmrtsInventory &inventory, 
                       const std::string &cache_name = psmrts_inventory) {
        return ( this->merge( inventory, cache_name ) );
      }
      
      /** Remove the requested cache value by key */
      inline size_t merge( const PsmrtsInventory &inventory, 
                           const std::string &cache_name ) {

        size_t n_merged = 0;
        if ( this->inventory().contains( cache_name ) ) {
          PsmrtsInventory &cache = this->inventory().find( cache_name );
          n_merged += cache.merge( inventory );
        }
        else {
          this->inventory().add( cache_name, inventory );
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
        m_inventory.remove( invname );
      }
    
      /** Liquidate/empty all PSRMTS factory inventory - affects all instances of PsmrtsFactory! */
      inline static void liquidate( ) {
        PsmrtsFactory::m_inventory.clear();
        
        // Be sure to set up the defaul inventory
        PsmrtsFactory::m_inventory = { "psmrts", "inventory", &FactoryInventory::case_insensitive_key  };
        return;
      }

    private:
    // Definitions for the product registry. This holds all the products that have specifications
      using ProductSpecs =  ProductInventory<std::string, ProductSpecification>;
      using ProductRegistry =  ProductInventory<std::string, ProductSpecs>;
      static inline ProductRegistry  m_registry  = { "psmrts", "registry", &ProductRegistry::case_insensitive_key };

      // Definitions and cache of active product inventories.
      using FactoryInventory = ProductInventory<std::string, PsmrtsInventory>;
      static inline FactoryInventory m_inventory = { "psmrts", "inventory", &FactoryInventory::case_insensitive_key  }; // set up default product cache

      /** Return the factory inventory */
      inline const FactoryInventory &inventory() const {
        return ( PsmrtsFactory::m_inventory );
      }

      /** Return the factory inventory */
      inline FactoryInventory &inventory()  {
        return ( PsmrtsFactory::m_inventory );
      }

  };

} // namespace psmrts

#endif
