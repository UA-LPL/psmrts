#ifndef PsmrtsCache_hpp
#define PsmrtsCache_hpp

#include <string>
#include <sstream>  
#include <map>
#include <algorithm>
#include <iterator>
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
   * @tparam K Key to use in the cache map
   * @tparam T Type stored in the cache map
   */
  template <typename K, typename T>
    class PsmrtsCache {
      public:
        using UIDType           = K;  // Define the unique identifier of the map key
        using CacheMap          = std::map<K,T>;
        using CacheMapIter      = typename std::map<K,T>::iterator;
        using CacheMapConstIter = typename std::map<K,T>::const_iterator;

        PsmrtsCache( ) : m_cache(), m_mutex() {  }
        PsmrtsCache( const std::string &name ) : 
                     m_cache(), 
                     m_mutex() {  }
        /** Required copy constructor due to std::mutex */
        PsmrtsCache( const PsmrtsCache &other )  {
          std::scoped_lock( other.mutex() );
          m_cache = other.m_cache; 
        }

        /** Required copy operator due to std::mutex */
        PsmrtsCache &operator=( const PsmrtsCache &other ) {
          if (this != &other) {
            std::scoped_lock mylocker( m_mutex, other.mutex() );
            m_cache = other.m_cache;
          }
          return ( *this );
        }
        virtual ~PsmrtsCache() { }

        /** Returns the number of elements in the cache */
        inline size_t size() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_cache.size() );
        }

        /** Add a value to the cache - overwrites existing data */
        inline void add( const K &key, const T &value ) {
          std::scoped_lock mylocker( this->mutex() );
          m_cache[key] = value;
        }


        /** Remove the requested cache value by key */
        inline void remove( const K &key ) {
          std::scoped_lock mylocker( this->mutex() );
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            m_cache.erase( it_c );
          }
          return;
        }

        /** Check for a particular key/value in the cache */
        inline bool contains( const K &key ) const {
          std::scoped_lock mylocker( this->mutex() );
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) return ( true );
          return ( false );
        }

        /** Find and return a reference to the specified key value */
        inline T &find( const K &key ) {
          std::scoped_lock mylocker( this->mutex() );
          CacheMapIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            return ( it_c->second );
          }
          
          // Throw an error if not found
          std::ostringstream mess_s;
          mess_s << "*** Error - PsmrtsCache::find(" << key << ") not found!";
          throw std::runtime_error( mess_s.str() );          
        }

        /** Find and return a reference to the specified key value */
        inline const T &find( const K &key ) const {
          std::scoped_lock mylocker( this->mutex() );
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            return ( it_c->second );
          }
          
          // Throw an error if not found
          std::ostringstream mess_s;
          mess_s << "*** Error - PsmrtsCache:find(" << key << ") not found!";
          throw std::runtime_error( mess_s.str() );
        }

        /** Return a key value if it exits otherwise returns the default value */
        inline const T &find( const K &key, const T &default_t ) const {
          std::scoped_lock mylocker( this->mutex() );
          CacheMapConstIter it_c = m_cache.find( key );
          if ( it_c != m_cache.end() ) {
            return ( it_c->second );
          }
          return ( default_t );
        }

        /** Return the const begin iterator of the map */
        inline CacheMapIter begin() {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_cache.begin() );
        }

        /** Return the const end iterator of the map */
        inline CacheMapIter end() {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_cache.end() );
        }

        /** Return the const begin iterator of the map */
        inline CacheMapConstIter begin() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_cache.cbegin() );
        }

        /** Return the const end iterator of the map */
        inline CacheMapConstIter end() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_cache.cend() );
        }

        inline void clear() {
          std::scoped_lock mylocker( this->mutex() );
          m_cache.clear();
        }

        inline std::vector<K> keys() const {
          std::scoped_lock mylocker( this->mutex() );
          std::vector<K> keys_m;
          keys_m.reserve( m_cache.size() );
          std::transform( m_cache.begin(), m_cache.end(), std::back_inserter( keys_m ), 
                          []( const auto &kv_t) { return ( kv_t.first ); } );
          return ( keys_m );
        }

        inline std::vector<T> values() const {
          std::scoped_lock mylocker( this->mutex() );
          std::vector<T> values_m;
          values_m.reserve( m_cache.size() );
          std::transform( m_cache.begin(), m_cache.end(), std::back_inserter( values_m ), 
                          []( const auto &kv_t ) { return ( kv_t.second ); } );
          return ( values_m );
        }

      protected:
        /** Mutex for thread locking use - local data only! */
        inline std::mutex &mutex() const {
          return ( m_mutex );
        }

      private:
        CacheMap           m_cache;
        mutable std::mutex m_mutex;
    };

} // namespace psmrts

#endif
