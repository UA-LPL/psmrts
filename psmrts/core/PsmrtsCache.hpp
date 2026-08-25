/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsCache_hpp
#define PsmrtsCache_hpp

#include <string>
#include <sstream>  
#include <map>
#include <algorithm>
#include <iterator>
#include <functional>
#include <mutex>
#include <shared_mutex>

namespace psmrts {

  /**
   * @brief Template class for thread-safe caching PSMRTS data objects/elements
   * 
   * This class provides caching of PSMRTS data using K key-based maps for
   * any T data/object. Each instance of the cache is mostly thread-safe,
   * however, const and non-const references to existing cached values are
   * returned.
   * 
   * Note that when the cache is copied, the data is deep copied. As such a 
   * unique mutex is created for each copy and they are no longer considered the
   * same cache.
   * 
   * Note that this cache does not return any references but the actual value
   * so if it should not be used for large value types. It is designed mainly
   * for std::strings and mininally sized data objects.
   * 
   * @tparam K Key to use in the cache map
   * @tparam T Type stored in the cache map
   */
  template <typename K, typename T, typename Compare = std::less<K>>
    class PsmrtsCache {
      public:
        using CacheMap          = std::map<K,T,Compare>;
        using CacheMapIter      = typename std::map<K,T,Compare>::iterator;
        using ConstCacheMapIter = typename std::map<K,T,Compare>::const_iterator;

        // Iterator function/lamda directly on cache data container
        using IteratorFunction  = std::function<void( const CacheMap &cachemap )>;        

        PsmrtsCache( ) : m_name("cache"), m_cache(), m_mutex() {  }
        PsmrtsCache( const std::string &name ) :
                     m_name( name ), 
                     m_cache(), 
                     m_mutex() {  }
        /** Required copy constructor due to std::mutex */
        PsmrtsCache( const PsmrtsCache &other )  {
          std::unique_lock<std::shared_mutex> mylocker( other.m_mutex );
          m_name = other.m_name;
          m_cache = other.m_cache; 
        }

        /** Required copy operator due to std::mutex */
        PsmrtsCache &operator=( const PsmrtsCache &other ) {
          if (this != &other) {
            std::unique_lock<std::shared_mutex> mylocker( other.m_mutex );
            m_name = other.m_name;
            m_cache = other.m_cache;
          }
          return ( *this );
        }
        virtual ~PsmrtsCache() = default;
        
        /** Returns name of this cache */
        inline const std::string &name() const {
          return ( m_name );
        }

        /** Returns the number of elements in the cache */
        inline size_t size() const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          return ( m_cache.size() );
        }

        /** Add a value to the cache - overwrites existing data */
        inline K add( const K &key, const T &value ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          m_cache[key] = value;
          return ( key );
        }


        /** Remove the requested cache value by key */
        inline void remove( const K &key ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          CacheMapIter it_m = m_cache.find( key);
          if (it_m != m_cache.end() ) {
            m_cache.erase( it_m );
          }
          return;
        }

        /** Check for a particular key/value in the cache */
        inline bool contains( const K &key ) const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          return ( this->has_key( key ) );
        }

        /** Thread-safe process iterator function/lambda/object method */
        template <typename Functor>
          bool process( Functor function ) const {
            std::shared_lock<std::shared_mutex> mylocker( m_mutex );
            return ( function( m_cache ) );
          }

        /** Return a key value if it exits otherwise returns the default value */
        inline T find( const K &key ) const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          ConstCacheMapIter it_m = m_cache.find( key);
          if (it_m != m_cache.end() ) {          
            return ( it_m->second );
          }

          // Throw an error if not found
          std::ostringstream mess_s;
          mess_s << "*** Error - PsmrtsCache::find(" << key << ") not found!";
          throw std::runtime_error( mess_s.str() );         
        }

        /** Return a key value if it exits otherwise returns the default value */
        inline T find( const K &key, const T &default_t ) const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          ConstCacheMapIter it_m = m_cache.find( key);
          if (it_m != m_cache.end() ) {          
            return ( it_m->second );
          }
          return ( default_t );
        }

        inline void clear() {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          m_cache.clear();
        }

        inline std::vector<K> keys() const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          std::vector<K> keys_m;
          keys_m.reserve( m_cache.size() );
          std::transform( m_cache.begin(), m_cache.end(), std::back_inserter( keys_m ), 
                          []( const auto &kv_t) { return ( kv_t.first ); } );
          return ( keys_m );
        }

        inline std::vector<T> values() const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          std::vector<T> values_m;
          values_m.reserve( m_cache.size() );
          std::transform( m_cache.begin(), m_cache.end(), std::back_inserter( values_m ), 
                          []( const auto &kv_t ) { return ( kv_t.second ); } );
          return ( values_m );
        }

        /**
         * @brief Merge another cache into this one 
         * 
         * This method will merge a cache of the same type into this cache if
         * the key/value pair does not exist in this cache.
         * 
         * @param cache   Add the contents of this cache
         * @return size_t Number of products successfully added
         */
        inline size_t merge( const PsmrtsCache &cache ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          size_t n_merged = 0;

          /** Merger lambda function */
          auto merger_functor = [&]( const CacheMap &map_c ) -> bool {
            for ( const auto &[ uid, p ] : map_c ) {
              if ( !this->has_key( uid ) ) {
                m_cache[uid] = p;
                n_merged++;
              }              
            }
            return ( true );
          };

          // Do attempt to merge onto self
          if ( &cache != this  ) {
            cache.process( merger_functor );
          }

          return ( n_merged );
        }        

      private:
        std::string               m_name;
        CacheMap                  m_cache;
        mutable std::shared_mutex m_mutex;

        /**
         * @brief Check for a particular key/value in the cache
         * 
         * This method assumes another method is managing the cache 
         * lock. It searches for the existance of a keyed value.
         * 
         * @param key    Name of key to search 
         * @return true  If it exists
         * @return false If it doesn't exist
         */
        inline bool has_key( const K &key ) const {
          ConstCacheMapIter it_m = m_cache.find( key);
          if (it_m != m_cache.end() ) return ( true );
          return ( false );
        } 
        
    };

} // namespace psmrts

#endif
