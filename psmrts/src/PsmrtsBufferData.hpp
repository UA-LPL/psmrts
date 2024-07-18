#ifndef PsmrtsBufferData_hpp
#define PsmrtsBufferData_hpp

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <exception>

namespace psmrts {
/**
 * @brief Provides fundamental byte memory of arbitrary data types
 *
 * This class provides the fundamentals of storage for a generic data.

 * @author Kris J. Becker, University of Arizona
 * @history 2024-07-18 Kris J. Becker  Original Version
 */
  class PsmrtsBufferData {
    public:
      typedef uint8_t                     value_type;
      typedef value_type&                 reference;
      typedef const value_type&           const_reference;
      typedef value_type*                 pointer;
      typedef const value_type*           const_pointer;
      typedef std::ptrdiff_t              difference_type;
      typedef std::shared_ptr<value_type> shared_pointer_type; 

      /** Default constructor */
      PsmrtsBufferData() {
        init();
      }

      /** Construct an array of values */
      PsmrtsBufferData( const size_t n_data ) {
        init();
        allocate( n_data );
      }

      /** Construct an array of values */
      PsmrtsBufferData( const shared_pointer_type &data, 
                        const size_t n_data ) :
                        m_data( data ), 
                        m_data_ptr( data.get() ),
                        m_size( n_data ) {
      }


      /** User defined map to n_data T values where total_allocated() = ( value_size() * size() )*/
      PsmrtsBufferData( const value_type *data, const size_t n_data ) {
        init();
        m_data_ptr = data;
        m_size = n_data;
      }

      /** Construct a safe slice with no new memory allocation into original data buffer */
      PsmrtsBufferData( const PsmrtsBufferData &data, 
                        const size_t start_index = 0, 
                        const size_t n_data = 0 ) :
                        m_data( data.m_data ),
                        m_data_ptr( data.m_data_ptr ),
                        m_size( data.m_size ) {

        // Determine the number of values to map
        size_t n_values = n_data;
        if ( 0 == n_values ) n_values = data.size() - start_index;

        // Validate and update map
        try {
          data.validate( start_index );
          data.validate( start_index + n_values - 1 );
        }
        catch ( const std::runtime_error &e ) {
          std::string msg = "Invalid segment into data slice!\n" + std::string( e.what() );
          throw std::runtime_error( msg );
        }

        // Ok, it checks out. adjust the required parameters
        m_data_ptr = data.get( start_index );
        m_size = n_values;
      }

      /** Destructor */
      virtual ~PsmrtsBufferData() { }

      /** Total number of data T allocated */
      inline size_t size() const {
        return ( m_size );
      }

      inline const_pointer get( const size_t index = 0 ) const {
        return ( m_data_ptr + index );
      }

      inline pointer get( const size_t index = 0) {
        return ( m_data_ptr + index );
      }

      /** Returns a copy of the T value at the given index */
      inline reference ref( const int index )  {
        return ( m_data_ptr[index] );
      }

      /** Returns a copy of the T value at the given index */
      inline const_reference ref( const int index ) const {
        return ( m_data_ptr[index] );
      }

      /** Returns a modifiable reference to data at the give index */
      inline reference operator()( const int index ) {
        return ( this->ref( index ) );
      }

      /** Returns a const reference to data at the give index */
      inline const_reference operator()( const int index ) const {
        return ( this->ref( index ) );
      }      

      /** Extract a slice from the original dataset */
      inline PsmrtsBufferData slice( const size_t start_index, 
                                      const size_t n_data = 0 ) const {
        return ( PsmrtsBufferData( *this, start_index, n_data ) );
      }

      /** Get a deep copy of the buffer */
      inline  PsmrtsBufferData deep_copy( ) const {
        PsmrtsBufferData copy_t( this->size() );
        for ( size_t n = 0 ; n < this->size() ; n++ ) {
          copy_t( n ) = this->ref( n );
        }
        return ( copy_t );
      }
    
      /** Compute distance of index to origin of the dataset in type indexes */
      inline difference_type distance( const int index ) const {
        const_pointer origin = ( nullptr != m_data.get() ) ? m_data.get() : m_data_ptr ;
        return ( this->distance( origin, this->get( index ) ) );
      }

    
      /** Compute distance of index to origin of the dataset in type indexes */
      inline difference_type distance( const_pointer base,
                                       const_pointer data_p ) const {
        return ( data_p - base );
      }

      /** Validate the index into a T value */
      inline void validate( const int index ) const {
        if ( index >= m_size ) {
          std::string mess = "Invalid index ( " + std::to_string( index ) +
                              "), max index is " + std::to_string( m_size ) + " - 1";
          throw std::runtime_error( mess );
        }
      }

    protected:

      /** Reset all variables to default state which releases any prior data */
      inline void init() {

        m_data.reset();
        m_data_ptr  = m_data.get();
        m_size      = 0;

        return;
      }

      /** Allocate n_data byte elements */
      inline void allocate( const size_t n_data ) {
        try {
          size_t v_alloc = n_data;

          m_data      = shared_pointer_type ( new value_type[v_alloc],
                                              std::default_delete<value_type[]>() );
          m_data_ptr  = m_data.get();
          m_size      = n_data;
        }
        catch ( const std::bad_alloc &b_alloc ) {
          init();
          std::string msg = "Failed to allocate data buffer of size " +
                            std::to_string( n_data );
          throw std::runtime_error( msg + "\n" + b_alloc.what() );
        }

        return;
      }

    private:
      std::shared_ptr<value_type> m_data;        // Data array T scalar values
      const_pointer               m_data_ptr;    // This will allow for user
                                                  // defined data access
      size_t                      m_size;        // Number of values of T

  };

}  // namespace psmrts

#endif // PsmrtsBufferData_hpp
