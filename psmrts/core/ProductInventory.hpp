#ifndef ProductInventory_hpp
#define ProductInventory_hpp
#pragma once

#include <string>
#include <functional>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>

namespace psmrts {


  /** 
   * @brief PSMRTS system inventories
   * 
   * This is a PSMRTS Product inventory map container. This is intended to 
   * provide a set of products that can be used to describe a completely
   * configured and maintained keyword parameter/options, shape, tracer and
   * priority tracer product system.
   * 
   * Note that the key mapping function must be a static function and should
   * be initialized using the full template declared type. This is because
   * when ProductInventorys are copied, the function pointer is also copied.
   * This requires the function to exist beyond the scope of the object. I am
   * not sure this is directly enforceable and may be the source of wierd
   * behavior/errors. Non-capturing lambdas fufill this requirement.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-09-03 Kris J. Becker  Original Version
   */
  template <typename K, typename P>
    class ProductInventory : public PsmrtsProduct {
      public:
        using CacheType       = PsmrtsCache<K,P>;
        using UIDType         = typename CacheType::UIDType; // == K

        static std::string case_insensitive_key( const std::string &key ) {
          return ( psmrts_tolower( key ) );
        };

        /** Generic key translation function */
        static K get_real_map_key( const K &key ) {
          return ( key );
        }

    
        ProductInventory( ) : PsmrtsProduct( "product", "inventory" ),
                              m_cache(),
                              m_key_t{ &ProductInventory::get_real_map_key } { }
        ProductInventory( const std::string &product_name,
                          const std::string &itype = "inventory" ) : 
                          PsmrtsProduct( product_name, itype ), 
                          m_cache(),
                          m_key_t( &get_real_map_key ) { }

          /** This constructor requires a static function! */
          ProductInventory( std::function<K(const K)> &func  ) : 
                           PsmrtsProduct( "product", "inventory" ),
                           m_cache(),
                           m_key_t( func ) { } 
        
        
        template<typename KeyMapFunc>
          ProductInventory( const std::string &product_name,
                            const std::string &itype,
                            KeyMapFunc&& func ) : 
                            PsmrtsProduct( product_name, itype ), 
                            m_cache(),
                            m_key_t( std::forward<KeyMapFunc>(func) ) { }
        virtual ~ProductInventory() { }

              
        inline size_t size( ) const {
          return ( m_cache.size() );
        }

        /**
         * @brief Add a product to the cache using UID
         * 
         * This method will add a product to the cache.
         * Note that it will not add an existing product,
         * thus overwriting the current version. If that
         * is required, you must remove the current one
         * with this->remove( product ).
         * 
         * @param key      Cache key for product
         * @param product  Product to add to cache
         * @return UIDType Returns the unique cache ID
         *                   of the product
         */
        inline bool add( const K &key, const P &product ) {
          // NEVER replace existing products
          if ( !m_cache.contains( m_key_t( key ) ) ) {
             m_cache.add( m_key_t( key ), product );
             return ( true );
          }
          return ( false );
        }

        /** Add a PSMRTS product that has a required uid() method */
        inline K add_product( const P &product ) {
          auto const &key = product.uid();
          this->add( key, product );
          return ( key );
        }

        /**
         * @brief Merge another product inventory into this one
         * 
         * Note: @see add( const P &product ) for merge rules.
         * 
         * @param cache   Add the contents of this cache
         * @return size_t Number of products successfully added
         */
        inline size_t merge( const ProductInventory &product ) {
          size_t n_merged = 0;
          for ( auto const &p_it : product.cache() ) {
            if ( !m_cache.contains( p_it.first ) ) {
              m_cache.add( m_key_t( p_it.first ), p_it.second );
              n_merged++;
            }
          }
          return ( n_merged );
        }

        /** Check for the existance of a product with K=key  */
        inline bool contains( const K &key ) const {
          return ( m_cache.contains( m_key_t( key ) ) );
        }

        /** Return the product with K=key */
        inline P &find( const K &key ) {
          return ( m_cache.find( m_key_t( key ) ) );
        }

        /** Return the product with K=key */
        inline const P &find( const K &key ) const {
          return ( m_cache.find( m_key_t( key ) ) );
        }


        /** Return the product with K=key */
        inline const P &find_by_uid( const K &key ) const {
          return ( m_cache.find( m_key_t( key ) ) );
        }

        /** Remove the product with K=key */
        inline void remove( const K &key ) {
          m_cache.remove( m_key_t( key ) );
          return;
        }

        /** Remove the same products in this cache that exists in cache */
        inline void remove( const CacheType &cache ) {
          for ( auto const &p_it : m_cache ) {
            m_cache.remove( m_key_t( p_it->first ) );
          }
          return;
        }

        /** Const access direct to the PsmrtsCache for iterator use mainly */
        inline const CacheType &cache() const {
          return ( m_cache );
        }

        /**
         * @brief Just because this is exposed here doesn't you should use it
         * 
         * Be careful when using this methods as it releases all previous products.
         * This is likely OK as they are designed to be copyable without
         * consequence. This may also empty a cache that is used in systematic
         * processing which will likely result in fatal exceptions.
         */
        inline void clear() {
          m_cache.clear();
        }

      private:
        CacheType                 m_cache;  ///!  The product cache
        std::function<K(const K)> m_key_t;  ///!  Instance of map key translator



    };


    // Specialized template instantiation of case aware string key objects
    /**
     * @brief Create a case insensitive std:string key inventory object
     * 
     * This method constructor creates a case insensitive inventory object
     * where the cache key is a string and the case is converted to
     * lowercase when data is inserted into the map.
     * 
     * This type of cache can be used for named inventory such as ISIS
     * mission translation strings and general typed caches.
     * 
     * @tparam P     Type of data stored in the cache map
     * @param name   Name of the cache. This can be stored in the
     *                 PsmrtsFactory as a product repository. 
     * @param itype   Type of the product stored
     * @return ProductInventory<typename P> Case insensitve cache map where
     *                   the key is stored as a lower case string
     */
    template <typename P> 
      static ProductInventory<std::string, P> 
          create_case_insensitive_inventory( const std::string &name,
                                              const std::string &itype = "inventory") {
            return ( ProductInventory<std::string, P>(name, itype,
                                                      &ProductInventory<std::string, P>::case_insensitive_key ) );
          }

    /**
     * @brief Create a case sensitive std:string key inventory object
     * 
     * This method constructor creates a case sensitive inventory object
     * where the cache key is a string and the case is preserved when the
     * data is inserted into the map. 
     * 
     * This object is recommended for sorting environment variables (P =
     * std::string), absolute/expanded file names of PsmrtsShapes,
     * PsmrtsTracers and PsmrtsPriority tracers.
     * 
     * @tparam P     Type of data store in the cache map
     * @param name   Name of the cache. This can be stored in the
     *                 PsmrtsFactory as a product repository. 
     * @param itype  Type of the product stored
     * @return ProductInventory<typename P> Case sensitive cache map
     */
    template <typename P> 
      static ProductInventory<std::string, P> 
          create_case_sensitive_inventory( const std::string &name,
                                            const std::string &itype = "inventory") {
            return ( ProductInventory<std::string, P>(name, itype,
                                                      &ProductInventory<std::string, P>::get_real_map_key ) );
          }
          
              
    // Declare string type cache for case sensitive and insensitive map keys of
    // strings
    template <typename P>
      using CaseSensitivyKeyMap = ProductInventory<std::string, P>;

} // namespace psmrts

#endif
