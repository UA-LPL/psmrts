#pragma once

#include <vector>
#include <string>
#include <exception>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>

namespace psmrts { 

  /**
   * @brief PRQProduct is a request to create products through configurations
   * 
   * This class accepts a PSRMTS products configuration request and 
   * fulfill the request by navigating system resources and utilizing
   * existing products. It contains a local inventory that can be used
   * to restrict the scope of searches for existing product to the local
   * cache. This implies the local cache is always used first to search
   * for products with similar configurations. If the product request
   * is not fully complete (determined my residual config options) the
   * search will then use system resources as contained from references
   * and other means as specfied in this PRQ.
   * 
   * Users can also specify the names of cached inventories stored in
   * the PSMRTS PsmrtsFactory system. If any names exist here, the system
   * "psmrts" inventory is excluded from the search (unless explicitly
   * added) for existing products.
   * 
   * Users can submit this request to PsrmtsFactory which will process the
   * request for general purposes. Custom processes can also be used where
   * needed.
   * 
   * @author 2025-09-20 Kris J Becker, University of Arizona
   * @history 2025-09-20 Kris J Becker, Original Version
   * 
   */
  class PRQProduct : public PsmrtsRequest {
    public:

    /** default constructable */
      PRQProduct() : PsmrtsRequest( "PRQProduct" ) { 
        init();
      }
      PRQProduct( const std::string &name ) : 
                  PsmrtsRequest( name ) {
        init( name );
      }
      PRQProduct( const std::string &name,
                  const ProductConfiguration &product_c ) : 
                  PsmrtsRequest( name ) { 
        init( name );
        m_config = product_c;
      }
      PRQProduct(  const std::string &name,
                   const PsmrtsInventory &local ) : 
                   PsmrtsRequest( name ) { 
        init( name );
        m_local_inventory = local;
      }      
      virtual ~PRQProduct() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      /**
       * @brief Adds a named inventory to the list of inventories to search
       * 
       * This method is provided to allow users to expand the scope of product
       * searches to other existing inventories beyond the PSMRTS default system
       * inventory. The list of inventories will be queried in the order listed
       * so as to prioritize searches of existing products.
       * 
       * Using this approach will reduce memory usage.
       * 
       * See also add_inventory( const PsmrtsInventory &inventory).
       * 
       * @param inventory_name Name of an inventory that is contained in the
       *                         PSMRTS inventory cache system 
       */
      inline void add_inventory( const std::string &inventory_name ) {
        m_inventory_names.push_back( inventory_name );
      }

      /** Set the product configuration */
      inline void set_config( const ProductConfiguration &config ) {
        m_config = config;
      }

      /** Returns a reference to the product configuration */
      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      /**
       * @brief Adds/merges an inventory into the local inventory
       * 
       * This method takes an inventory that is added (merged) into the
       * local inventory that is used as the preferred inventory to query
       * for configured products. For any query, it will first use this
       * combined inventory to search for and match existing products
       * for reuse.
       * 
       * This inventory can be added or merged back into the system cache
       * for further use.
       * 
       * @param inventory Add/merge this inventory into the local inventory
       */
      inline void add_inventory( const PsmrtsInventory &inventory ) {
        m_local_inventory.merge( inventory );
      }

      /** Returns const reference to local PSMRTS inventory cache */
      inline const PsmrtsInventory &local_inventory() const {
        return ( m_local_inventory );
      }

      /** Returns the product inventory */
      inline const PsmrtsInventory &product_inventory() const {
        return ( m_product );
      }

      /** Add a shape to the inventory */
      inline bool add_shape( const PsmrtsShape &shape ) {
        return ( m_product.shapes().add_product( shape ) );
      }

      /** Add a tracer to the inventory */
      inline bool add_tracer( const PsmrtsTracer &tracer ) {
        return ( m_product.tracers().add_product( tracer ) );
      }

      /** Return list of active inventories */
      inline const std::vector<std::string> &get_inventory_list() const {
        return ( m_inventory_names );
      }

    private:
      std::vector<std::string> m_inventory_names;
      ProductConfiguration     m_config;
      PsmrtsInventory          m_local_inventory;
      PsmrtsInventory          m_product;

      inline void init( const std::string &name = "PRQRequest" ) {
        m_inventory_names.clear();
        m_config          = ProductConfiguration( name );
        m_local_inventory = PsmrtsInventory( "local" );
        m_product         = PsmrtsInventory( name );
      }

  };

}  // namespace psmrts
