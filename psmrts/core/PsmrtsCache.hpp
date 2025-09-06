#ifndef PsmrtsCache_hpp
#define PsmrtsCache_hpp

#include <string>
#include <iostream>   
#include <sstream>  
#include <exception>
#include <map>

namespace psmrts {

  /**
   * @brief Template class for caching PSMRTS data objects/elements
   * 
   * This class provides caching of PSMRTS data using K key-based maps for
   * any T data/object. 
   * 
   * @tparam K Key to use in the cache map
   * @tparam T Type stored in the cache map
   */
  template <typename K, typename T>
    class PsmrtsCache {
      public:
        using CacheMap          = std::map<K,T>;
        using CacheMapIter      = typename std::map<K,T>::iterator;
        using CacheMapConstIter = typename std::map<K,T>::const_iterator;

        PsmrtsCache( )  {  }
        virtual ~PsmrtsCache() { }

        /** Returns the number of elements in the cache */
        inline size_t size() const {
          return ( m_cache.size() );
        }

        /** Add a value to the cache - overwrites existing data */
        inline void add( const K &key, const T &value ) {
          m_cache.insert( { key, value } );
        }

        /** Remove the requested cache value by key */
        inline void remove( const K &key ) {
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            m_cache.erase( it_c );
          }
          return;
        }

        /** Check for a particular key/value in the cache */
        inline bool contains( const K &key ) const {
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) return ( true );
          return ( false );
        }

        /** Find and return a reference to the specified key value */
        inline const T &find( const K &key ) const {
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            return ( it_c->second );
          }
          
          // Throw an error if not found
          std::ostringstream mess_s;
          mess_s << "*** Error - PsmrtsCache cache key " << key << " not found!";
          throw std::runtime_error( mess_s.str() );
        }

        /** Return a key value if it exits otherwise returns the default value */
        inline const T &find( const K &key, const T default_t ) const {
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            return ( it_c->second );
          }
          return ( default_t );
        }

        /** Return the const begin iterator of the map */
        inline CacheMapIter begin() {
          return ( m_cache.begin() );
        }

        /** Return the const end iterator of the map */
        inline CacheMapIter end() {
          return ( m_cache.end() );
        }

        /** Return the const begin iterator of the map */
        inline CacheMapConstIter cbegin() const {
          return ( m_cache.cbegin() );
        }

        /** Return the const end iterator of the map */
        inline CacheMapConstIter cend() const {
          return ( m_cache.cend() );
        }

        inline void clear() {
          m_cache.clear();
        }

      private:
        CacheMap  m_cache;
    };

} // namespace psmrts

#endif
