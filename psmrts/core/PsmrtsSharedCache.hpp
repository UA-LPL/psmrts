/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsSharedCache_hpp
#define PsmrtsSharedCache_hpp

#include <string>
#include <algorithm>
#include <iterator>
#include <functional>
#include <map>
#include <mutex>
#include <sstream>  

#include <shared_mutex>

namespace psmrts {


  /**
   * @brief Template class for thread-safe caching PSMRTS data objects/elements
   * 
   * This class provides caching of PSMRTS data using K key-based maps for
   * any T data/object. 
   *
   * The values are stored as shared pointers. You can add a stack instance 
   * (recommended) of a value T or a std::shared_ptr<T> instance. 
   * 
   * Find methods return a std::shared_ptr<T> value that will contain the
   * requested value by key if it exists, otherwise it will return an empty
   * shared pointer (i.e., std::shared_ptr<T>( nullptr )).
   * 
   * A template method, PsmrtsSharedCache::process(const CacheMap &map_c ),is 
   * provided that accepts a function, lambda or functor object that is called
   * with a const reference to the cache map.
   * 
   * @tparam K Key to use in the cache map
   * @tparam T Type stored in the cache map
   */
  template <typename K, typename T, typename Compare = std::less<K>>
    class PsmrtsSharedCache {
      public:
        using SharedType        = typename std::shared_ptr<T>;
        using ConstSharedType   = typename std::shared_ptr<const T>;
        using CacheMap          = std::map<K,SharedType,Compare>;
        using CacheMapIter      = typename std::map<K,SharedType,Compare>::iterator;
        using ConstCacheMapIter = typename std::map<K,SharedType,Compare>::const_iterator;

        // Iterator function/lamda directly on cache data container
        using IteratorFunction  = std::function<void( const CacheMap &cachemap )>;        

        PsmrtsSharedCache( ) : m_name("sharedcache"), m_cache(), m_mutex() {  }
        PsmrtsSharedCache( const std::string &name ) : 
                     m_name( name ), 
                     m_cache(), 
                     m_mutex() {  }
        /** Required copy constructor due to std::mutex */
        PsmrtsSharedCache( const PsmrtsSharedCache &other )  {
          std::unique_lock<std::shared_mutex> mylocker( other.m_mutex );
          m_name = other.m_name;
          m_cache = other.m_cache; 
        }

        /** Required copy operator due to std::mutex */
        PsmrtsSharedCache &operator=( const PsmrtsSharedCache &other ) {
          if (this != &other) {
            std::unique_lock<std::shared_mutex> mylocker( other.m_mutex );
            m_name = other.m_name;
            m_cache = other.m_cache;
          }
          return ( *this );
        }
        virtual ~PsmrtsSharedCache() = default;

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
        inline void add( const K &key, const T &value ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          m_cache[key] = std::make_shared<T>( std::move( value ) );
        }

        /** Add a value to the cache - overwrites existing data */
        inline void add( const T &value ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          m_cache[value.uid()] = std::make_shared<T>( std::move( value ) );
        }        

        /** Add a value to the cache - overwrites existing data */
        inline void add( const K &key, const SharedType &value ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          m_cache[key] = value;
        }        

        /** Remove the requested cache value by key */
        inline void remove( const K &key ) {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          CacheMapIter it_m = m_cache.begin();
          while ( it_m != m_cache.end() ) {
            if ( m_cache.key_comp()( key, it_m->first ) ) {
              m_cache.erase( it_m );
              break;
            }
            ++it_m;
          }
          return;
        }

        /** Check for a particular key/value in the cache */
        inline bool contains( const K &key ) const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          for ( const auto &[ uid, value_s ] : m_cache ) {
            if ( m_cache.key_comp()( key, uid ) ) return ( true );
          }
          return ( false );
        }

        /** Find and return the specified key value */
        inline SharedType find( const K &key ) const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          for ( const auto &[ uid, value_s ] : m_cache ) {
            if ( m_cache.key_comp()( key, uid ) ) return ( value_s );
          }
          return ( nullptr );
        }

        /** Thread-safe process iterator function/lambda/object method */
        template <typename Functor>
          bool process( Functor function ) const {
            std::shared_lock<std::shared_mutex> mylocker( m_mutex );
            return ( function( m_cache ) );
          }

          /** Clear out the entire cache */
        inline void clear() {
          std::unique_lock<std::shared_mutex> mylocker( m_mutex );
          m_cache.clear();
        }

        /** Return a list of the keys */
        inline std::vector<K> keys() const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          std::vector<K> keys_m;
          keys_m.reserve( m_cache.size() );
          std::transform( m_cache.begin(), m_cache.end(), std::back_inserter( keys_m ), 
                          []( const auto &kv_t) { return ( kv_t.first ); } );
          return ( keys_m );
        }

        /** Return a list of the all the values */
        inline std::vector<SharedType> values() const {
          std::shared_lock<std::shared_mutex> mylocker( m_mutex );
          std::vector<SharedType> values_m;
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
        inline size_t merge( const PsmrtsSharedCache &cache ) {
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
          for ( const auto &[ uid, value_s ] : m_cache ) {
            if ( m_cache.key_comp()( key, uid ) ) return ( true );
          }
          return ( false );
        }  
        
       /**
         * @brief Lock-free search for for a const value type
         * 
         * Returns an const iterator of the map entry using the key. This method
         * is lock-free and assumes the caller has applied any appropriate locks.
         * 
         * @param key           Map key to find entry for
         * @return CacheMapIter The iterator of the found map entry or end() if
         *                         not found
         */
        inline ConstCacheMapIter find_with_iter( const K &key ) const {
          ConstCacheMapIter it_m = m_cache.cbegin();
          while ( it_m != m_cache.cend() ) {
            if ( m_cache.key_comp()( key, it_m->first ) ) {
              break;
            }
            ++it_m;
          }

          return ( it_m );
        }

        /**
         * @brief Lock-free search for for a value type
         * 
         * Returns an iterator of the map entry using the key. This method is
         * lock-free and assumes the caller has applied any appropriate locks.
         * 
         * @param key           Map key to find entry for
         * @return CacheMapIter The iterator of the found map entry or end() if
         *                         not found
         */
        inline CacheMapIter find_with_iter( const K &key ) {
          CacheMapIter it_m = m_cache.begin();
          while ( it_m != m_cache.end() ) {
            if ( m_cache.key_comp()( key, it_m->first ) ) {
              break;
            }
            ++it_m;
          }

          return ( it_m );
        }

        
    };

} // namespace psmrts

#endif
