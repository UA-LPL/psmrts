#ifndef PsmrtsDataModel_hpp
#define PsmrtsDataModel_hpp

#include <cstddef>
#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsVector3.hpp>

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


#if 1
  template <typename T = double> 
    using PsmrtsDataModel = PsmrtsVector3<T>;
#else 
  template <typename T = double>
    class PsmrtsDataModel : public PsmrtsVector3<T> {
      public:
        typedef T                           value_type;
        typedef value_type&                 reference;
        typedef const value_type&           const_reference;
        typedef value_type*                 pointer;
        typedef const value_type*           const_pointer;
        typedef std::ptrdiff_t              difference_type;

        /** Default constructor */
        PsmrtsDataModel() : PsmrtsVector3<T>( ) {
          this->init_model();
        }

        /** Construct an array of values */
        PsmrtsDataModel( const size_t n_data ) :
                         PsmrtsVector3<T>( n_data, ( 3 * sizeof( value_type ) ) ) {
          this->allocate( n_data );
          this->init_model();
        }

        /** User defined map to n_data T values where total_allocated() = ( value_size() * size() )*/
        PsmrtsDataModel( const value_type *data, 
                         const size_t n_data ) :
                         PsmrtsVector3<T>( data, n_data, ( 3 * sizeof( value_type ) ) ) {
          this->init_model();            
        }

        /** Construct a safe slice with no new memory allocation into original data buffer */
        PsmrtsDataModel( const PsmrtsDataModel &data, 
                         const size_t start_index, 
                         const size_t n_data = 0 ) :
                         PsmrtsVector3<T>( data, start_index, n_data ) {
          this->init_model();                          
        }

        /** Destructor */
        virtual ~PsmrtsDataModel() { }

        /** Total number of data T allocated */
        inline size_t size() const {
          return ( this->buffer().size() );
        }

        /** Returns the number of values in T */
        inline size_t data_size() const {
          return (  );
        }

        /** Returns the number of bytes per T value */
        inline size_t scalar_size() const {
          return ( sizeof ( value_type ) );
        }

        /** Returns a copy of the T value at the given index */
        inline value_type at( const int index ) const {
          return ( value_type( get_data_ref( index ) ) );
        }

        /** Returns a modifiable reference to data at the give index */
        inline reference operator()( const int index ) {
          return ( reference( get_data_ref( index ) ) );
        }

        /** Returns a modifiable reference to data at the give index */
        inline const_reference ref( const int index ) {
          return ( const_reference( get_data_ref( index ) ) );
        }
        /** Returns a const reference to data at the give index */
        inline const_data_reference operator()( const int index ) const {
          return ( const_data_reference( get_data_ref( index ) ) );
        }

        /** Returns a const reference to data at the give index */
        inline const_data_reference ref( const int index ) const {
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

    };
#endif
}  // namespace psmrts

#endif // PsmrtsDataModel_hpp
