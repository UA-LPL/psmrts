#ifndef PsmrtsDataModel_hpp
#define PsmrtsDataModel_hpp

#include <string>
#include <memory>
#include <exception>
#include <Eigen/Geometry>

namespace psmrts {
/**
 * @brief PsmrtsDataModel provides general storage needs for arbitrary data types
 *
 * This class provides the fundamentals of storage for a mesh-type, tessellated
 * plate model. It is designed to store the facet index (integer), vector
 * (double or float) or can also store most other arbitrary types.
 *
 * This design uses the Eigen data type to map the second dimension to make
 * it usable directly in vector oriented systems.
 *
 * @author Kris J. Becker, University of Arizona
 * @history 2023-12-12 Kris J. Becker  Original Version
 */

  template <typename T = Eigen::Vector3d>
    class PsmrtsDataModel {
      public:
        typedef T                    data_type;
        typedef typename T::Scalar   value_type;

        typedef Eigen::Map<T>        data_reference;
        typedef Eigen::Map<const T>  const_data_reference;

        /** Default constructor */
        PsmrtsDataModel() {
          init();
        }

        /** Construct an array of values */
        PsmrtsDataModel( const size_t n_data ) {
          init();
          allocate( n_data );
        }

        /** User defined map to n_data T values where total_allocated() = ( value_size() * size() )*/
        PsmrtsDataModel( value_type *data, const size_t n_data ) {
          init();
          m_data_ptr = data;
          m_t_size = n_data;
          m_volume_size = n_data * data_size();
        }

        /** Construct a safe slice with no new memory allocation into original data buffer */
        PsmrtsDataModel( const PsmrtsDataModel &data, 
                         const size_t start_index, 
                         const size_t n_data = 0 ) :
                         m_data( data.m_data ),
                         m_data_ptr( data.m_data_ptr ),
                         m_t_size( data.m_t_size ),
                         m_values_size( data.m_values_size ),
                         m_volume_size( data.m_volume_size ) {

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
          m_data_ptr = data.get_data_ref( start_index );
          m_t_size = n_values;
        }

        /** Destructor */
        virtual ~PsmrtsDataModel() { }

        /** Total number of data T allocated */
        inline size_t size() const {
          return ( m_t_size );
        }

        /** Returns the number of values in T */
        inline size_t data_size() const {
          return ( m_values_size );
        }

        /** Returns the number of bytes per T value */
        inline size_t scalar_size() const {
          return ( sizeof ( value_type ) );
        }

        /** Returns the number of scalar values allocated */
        inline size_t total_allocated() const {
          return ( m_volume_size );
        }


        /** Returns a copy of the T value at the given index */
        inline T at( const int index ) const {
          return ( T( get_data_ref( index ) ) );
        }

        /** Returns a modifiable reference to data at the give index */
        inline data_reference operator()( const int index ) {
          return ( data_reference( get_data_ref( index ) ) );
        }

        /** Returns a const reference to data at the give index */
        inline const_data_reference operator()( const int index ) const {
          return ( const_data_reference( get_data_ref( index ) ) );
        }

        /** Extract a slice from the original dataset */
        inline PsmrtsDataModel slice( const size_t start_index, 
                                      const size_t n_data = 0 ) const {
          return ( PsmrtsDataModel( *this, start_index, n_data ) );
        }

        /** Get a deep copy of the buffer */
        inline  PsmrtsDataModel deep_copy( ) const {
          PsmrtsDataModel copy_t( this->size() );
          for ( size_t n = 0 ; n < this->size() ; n++ ) {
            copy_t( n ) = (*this)( n );
          }
          return ( copy_t );
        }

        /** Compute distance of index to origin of the dataset in type indexes */
        inline int distance( const int index ) const {
          const value_type *origin = ( nullptr != m_data.get() ) ? m_data.get() : m_data_ptr;
          const value_type *offset = get_data_index( index ) - origin;
          return ( offset / this->data_size() );
        }

      protected:

        /** Validate the index into a T value */
        inline void validate( const int index ) const {
          if ( index >= m_t_size ) {
            std::string mess = "Invalid index ( " + std::to_string( index ) +
                               "), max index is " + std::to_string( m_t_size ) + " - 1";
            throw std::runtime_error( mess );
          }
        }

        /** Compute the value_type index into T data volume */
        inline int get_data_index( const int index ) const {
#if defined( DEBUG ) || defined(PSMRTS_BOUNDS_CHECK)
          validate( index );
#endif
          return ( index * m_values_size );
        }

        /** Return modifiable memory reference of T at index */
        inline value_type *get_data_ref( const int index ) {
          return ( m_data_ptr + get_data_index( index ) );
        }

        /** Return const memory reference of T at index */
        inline const value_type *get_data_ref( const int index ) const {
          return ( m_data_ptr + get_data_index( index ) );
        }

        /** Reset all variables to default state which releases any prior data */
        inline void init() {
          m_data.reset();
          m_data_ptr    = m_data.get();

          m_values_size = T().size();
          m_t_size      = 0;
          m_volume_size = 0;
          return;
        }

        /** Allocate n_data T elements */
        inline void allocate( const size_t n_data ) {
          try {
            size_t v_alloc = n_data * m_values_size;

            m_data = std::shared_ptr<value_type> ( new value_type[v_alloc],
                                                   std::default_delete<value_type[]>() );
            m_data_ptr    = m_data.get();

            m_t_size      = n_data;
            m_volume_size = v_alloc;
          }
          catch ( const std::bad_alloc &b_alloc ) {
            init();

            std::string msg = "Failed to allocate data of size " +
                              std::to_string( n_data );

            throw std::runtime_error( msg + "\n" + b_alloc.what() );
          }

          return;
        }

      private:
        std::shared_ptr<value_type> m_data;        // Data array T scalar values
        value_type                 *m_data_ptr;   // This will allow for 1-based
                                                   // and user defined data access
        size_t                      m_values_size; // Number of value_types per T
        size_t                      m_t_size;      // Number of values of T
        size_t                      m_volume_size; // m_values_size * m_t_size


    };
}  // namespace psmrts

#endif // PsmrtsDataModel_hpp
