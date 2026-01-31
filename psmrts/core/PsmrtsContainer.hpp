#ifndef PsmrtsContainer_hpp
#define PsmrtsContainer_hpp

#include <string>
#include <sstream>  
#include <vector>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include <mutex>
#include <exception>

namespace psmrts {

  /**
   * @brief Simple vector container class for effective storage/access
   * 
   * This class provides containers of PSMRTS data using string (name) for
   * any T data/object. T objects must contain a T::name() method that returns
   * the name of the object as a std:string. Case is not considered to promote
   * efficient use.
   * 
   * Access to the data elements is thread-safe, but the data elememnts are
   * returned by reference for efficiency.
   * 
   * @tparam T Type stored in the container
   */
  template <typename T>
    class PsmrtsContainer {
      public:
        using Container          = std::vector<T>;
        using ContainerIter      = typename Container::iterator;
        using ContainerConstIter = typename Container::const_iterator;

        PsmrtsContainer( ) : m_name( "data" ),
                             m_data(),
                             m_mutex() { }
        PsmrtsContainer( const std::string &name ) : 
                         m_name( name ),
                         m_data(), 
                         m_mutex() {  }
        PsmrtsContainer( const std::string &name, 
                         const std::initializer_list<T> &data ) :
                         m_name( name ),
                         m_data( Container( data.begin(), data.end() ) ),
                         m_mutex() { }
        PsmrtsContainer( const std::string &name, 
                         const std::vector<T> &data ) :
                         m_name( name ),
                         m_data( data ),
                         m_mutex() { } 

        /** Required copy constructor due to std::mutex */
        PsmrtsContainer( const PsmrtsContainer &other )  {
          std::scoped_lock mylocker( other.mutex() );
          m_name = other.m_name;
          m_data = other.m_data; 
        }

        /** Required copy operator due to std::mutex */
        PsmrtsContainer &operator=( const PsmrtsContainer &other ) {
          if (this != &other) {
            std::scoped_lock mylocker( m_mutex, other.mutex() );
            m_name = other.m_name;
            m_data = other.m_data;
          }
          return ( *this );
        }

        virtual ~PsmrtsContainer() = default;

        /** Returns the name of the option */
        inline const std::string &name() const {
          return ( m_name );
        }

        /** Number of elements in container */
        inline size_t size() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_data.size() );
        }

        /**
         * @brief Add a data to the container if it doesn't already exist
         * 
         * This method adds data to the container if it does not already exist.
         * If not present, the data is not added ( @see replace()). Otherwise it
         * is appended to the container.
         * 
         * @param data  Data to add if not present
         * @return True if data was appended and not present in the container
         *         False if the data is present and was not added to the container
         */
        inline bool add( const T &data ) {
          std::scoped_lock mylocker( this->mutex() );
          ContainerIter data_t = m_data.begin();
          while ( data_t != m_data.end() ) {
            if ( data.name() == data_t->name() ) {
              return ( false );
            }
            ++data_t;
          }   

          m_data.push_back( data );
          return ( true );
        }

        /**
         * @brief Add data to the container replacing existing data if present
         * 
         * This method will add data to the container A check is made to
         * determine if it already exists. If it exists, it is replaced. If its
         * not present, the data is appended to the container.
         * 
         * @param data  Data to add/replace in the container
         * @return True if data was replaced
         *         False if it was appended 
         */
        inline bool replace( const T &data ) {
          std::scoped_lock mylocker( this->mutex() );
          ContainerIter data_t = iterator_find( data.name() );
          if ( data_t != m_data.end() ) {
            *data_t = data;
            return ( true );
          }   

          m_data.push_back( data );
          return ( false );
        }

        /** Remove the specified data object */
        inline bool remove( const std::string &key ) {
          std::scoped_lock mylocker( this->mutex() );
          ContainerIter data_t = iterator_find( key );
          if ( data_t != m_data.end() ) {
            m_data.erase( data_t );
            return ( true );
          }
          return ( false );
        }
        
        /** Check for a particular key/value in the cache */
        inline bool contains( const std::string &key ) const {
          std::scoped_lock mylocker( this->mutex() );
          for ( const T &d : m_data ) {
            if ( key == d.name() ) return ( true );
          }
          return ( false );
        }

        /** Find and return a reference to the specified key value */
        inline const T &find( const std::string &key ) const {
          std::scoped_lock mylocker( this->mutex() );
          for ( const T &d : m_data ) {
            if ( key == d.name() ) return ( d );
          }
          // Throw an error if not found
          std::ostringstream mess_s;
          mess_s << "*** Error - PsmrtsContainer:find(" << key << ") not found!";
          throw std::runtime_error( mess_s.str() );
        }

        /** Return a key value if it exits otherwise returns the default value */
        inline const T &find( const std::string &key, 
                              const T &default_t ) const {
          std::scoped_lock mylocker( this->mutex() );
          for ( const T &d : m_data ) {
            if ( key == d.name() ) return ( d );
          }
          return ( default_t );
        }

        /** Return the const begin iterator of the map */
        inline ContainerConstIter begin() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_data.cbegin() );
        }

        /** Return the const end iterator of the map */
        inline ContainerConstIter end() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_data.cend() );
        }

        /** Clear the contents, which invalidates references */
        inline void clear() {
          std::scoped_lock mylocker( this->mutex() );
          m_data.clear();
        }

        /** Return list of keys associated with the data objects */
        inline std::vector<std::string> keys() const {
          std::scoped_lock mylocker( this->mutex() );
          std::vector<std::string> keys_m;
          keys_m.reserve( m_data.size() );
          std::transform( m_data.begin(), m_data.end(), std::back_inserter( keys_m ), 
                          []( const auto &kv_t) { return ( kv_t.name() ); } );
          return ( keys_m );
        }

        /** Return reference to data object vector */
        inline const Container &data() const {
          std::scoped_lock mylocker( this->mutex() );
          return ( m_data );
        }

      protected:
        /** Mutex for thread locking use - local data only! */
        inline std::mutex &mutex() const {
          return ( m_mutex );
        }

      private:
        std::string        m_name;
        Container          m_data;
        mutable std::mutex m_mutex;

        /** Return an iterator (no mutex locking!) data position associated with the key */
        inline ContainerIter iterator_find( const std::string &key  ) {
          ContainerIter data_t = m_data.begin();
          while ( data_t != m_data.end() ) {
            if ( key == data_t->name() ) {
              return ( data_t );
            }
            ++data_t;   
          }
          return ( m_data.end() );
        }
    };

} // namespace psmrts

#endif
