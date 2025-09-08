#ifndef ProductInventory_hpp
#define ProductInventory_hpp
#pragma once

#include <exception>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>

namespace psmrts {

  /**
   * @brief Template struct method for a noop UID key translation
   * 
   * Use this template method functor to simply pass the map key
   * as is with no translation to the actual real cached UIDType.
   * 
   * @tparam K Type of the UID to translate
   */
  template <typename K> 
    struct noop_key_id {
      inline K get_real_map_key( const K &key ) const {
        return ( key );
      }
    };

  /**
   * @brief Template struct method to enforce lowercase string UID keys
   * 
   * Use this template method functor to convert any incoming key from
   * the caller to lowercase. Note this is really only applicable if the
   * the key is std::string. This provides support to enforce internally
   * lowercase keys.
   * 
   * This serves as an example to emply more complex needs. Usage in this
   * way will typically be optimized well by the compiler.
   * 
   * @tparam K std::string in this specialization
   */  
  template <typename K = std::string> 
    struct lowercase_key_id {
      inline K get_real_map_key( const K &key ) const {
        return ( psmrts_tolower( key ) );
      }
    };    

  /** 
   * @brief PSMRTS system inventories
   * 
   * This is a PSMRTS Product inventory map container. This is intended to 
   * provide a set of products that can be used to describe a completely
   * configured and maintained keyword parameter/options, shape, tracer and
   * priority tracer product system.
   * 

   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-09-03 Kris J. Becker  Original Version
   */
    template <typename K, typename P, typename U=noop_key_id<K>>
    class ProductInventory : public PsmrtsProduct {
      public:
        using CacheType       = PsmrtsCache<K,P>;
        using UIDType         = PsmrtsUID::UIDType;
        using KeyToMapUID     = U;

        ProductInventory( ) : PsmrtsProduct( "product", "inventory" ),
                              m_cache() { }
        ProductInventory( const std::string &product_name,
                          const std::string &itype = "inventory" ) : 
                          PsmrtsProduct( product_name, itype ), 
                          m_cache() { }

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
          if ( !m_cache.contains( m_key_t.get_real_map_key( key ) ) ) {
             m_cache.add( m_key_t.get_real_map_key( key ), product );
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
              m_cache.add( m_key_t.get_real_map_key( p_it.first ), p_it.second );
              n_merged++;
            }
          }
          return ( n_merged );
        }

        /** Check for the existance of a product with K=key  */
        inline bool contains( const K &key ) const {
          return ( m_cache.contains( m_key_t.get_real_map_key( key ) ) );
        }

        /** Return the product with K=key */
        inline const P &find_by_uid( const K &key ) const {
          return ( m_cache.find( m_key_t.get_real_map_key( key ) ) );
        }

        /** Remove the product with K=key */
        inline void remove( const K &key ) {
          m_cache.remove( m_key_t.get_real_map_key( key ) );
          return;
        }

        /** Remove the same products in this cache that exists in cache */
        inline void remove( const CacheType &cache ) {
          for ( auto const &p_it : m_cache ) {
            m_cache.remove( m_key_t.get_real_map_key( p_it->first ) );
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
        CacheType   m_cache;  ///!  The product cache
        KeyToMapUID m_key_t;  ///!  Instance of map key translator
    };

    // Declare string type cache for case sensitive and insensitive map keys of strings
    using LowerCaseKeyMap     = ProductInventory<std::string,std::string,lowercase_key_id<std::string>>;
    using CaseSensitiveKeyMap = ProductInventory<std::string,std::string,noop_key_id<std::string>>;

} // namespace psmrts

#endif
