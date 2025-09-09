#ifndef PsmrtsFactory_hpp
#define PsmrtsFactory_hpp

#include <string>
#include <iostream>   
#include <sstream>  
#include <exception>
#include <map>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>

namespace psmrts {

  /**
   * @brief PSMRTS Factory provider of all products
   * 
   * This class contains a persistent inventory of all
   * PSMRTS products. Inventories can be created for any
   * situation and copied freely as standalone resources.
   * Using this class for all PSMRTS product quieries will
   * centralize the caching of all products during systematic
   * processing. 
   * 
   * This design also provides opportunities to create or
   * copy existing inventories for specialized/controlled
   * environments. This will also help aid in scaling
   * management of PSMRTS environments.
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


      inline size_t size() const {
        return ( this->inventory().size() );
      }

      /** Looking for an inventory by name */
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
      inline void add( const PsmrtsInventory &inventory, 
                       const std::string &cache_name = psmrts_inventory) {
        this->merge( inventory, cache_name );
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

      inline static void liquidate( ) {
        PsmrtsFactory::m_inventory.clear();
        return;
      }

    private:
      using FactoryInventory = ProductInventory<std::string, PsmrtsInventory, lowercase_key_id<std::string>>;
      static inline FactoryInventory m_inventory = { "psmrts", "inventory" };

      inline const FactoryInventory &inventory() const {
        return ( PsmrtsFactory::m_inventory );
      }

      inline FactoryInventory &inventory()  {
        return ( PsmrtsFactory::m_inventory );
      }

  };

} // namespace psmrts

#endif
