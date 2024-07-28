#ifndef PsmrtsVector3_hpp
#define PsmrtsVector3_hpp

#include <cstddef>
#include <string>
#include <exception>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsVector3.hpp>

namespace psmrts {
/**
 * @brief Provides template for generic type buffer and indexing
 *
 *
 * This class provides mappings of three-element vector data of arbitrary data
 * types and maps them as Eigen::Vector3s. The 3D vector is a major type that
 * occurs frequently in PSMRTS. This is common for int and float/double types
 * when manipulating meshes.
 * 
 * This class suppports data of arbitrary spacing for maximum utilty
 * 
 * @author Kris J. Becker, University of Arizona
 * @history 2024-07-22 Kris J. Becker  Original Version
 */
template <typename T>
  class PsmrtsVector3 : public PsmrtsBuffer<T> {
    public:
      typedef typename PsmrtsBuffer<T>::value_type            value_type;
      typedef typename PsmrtsBuffer<T>::reference             reference;
      typedef typename PsmrtsBuffer<T>::const_reference       const_reference;
      typedef typename PsmrtsBuffer<T>::pointer               pointer;
      typedef typename PsmrtsBuffer<T>::const_pointer         const_pointer;     

      typedef typename Eigen::Vector3<value_type>             vector_type;
      typedef typename Eigen::Map<vector_type>                vector_reference;
      typedef typename Eigen::Map<const vector_type>          const_vector_reference;

      typedef typename PsmrtsBuffer<T>::buffer_pointer        buffer_pointer;
      typedef typename PsmrtsBuffer<T>::const_buffer_pointer  const_buffer_pointer;      

      static constexpr size_t VectorSize  = vector_type::RowsAtCompileTime * vector_type::ColsAtCompileTime;
      static constexpr size_t VectorBytes = VectorSize * sizeof( T );

      /** Default constructor */
      PsmrtsVector3() : PsmrtsBuffer<T>( ) {  }

      /** Construct an array of T values */
      PsmrtsVector3( const size_t n_data ) : 
                     PsmrtsBuffer<T> ( n_data, VectorBytes ) {
        this->validate_state();
      }

      PsmrtsVector3( const PsmrtsVector3 &v ) = default;

      /** Construct slice of an existing buffer */
      explicit PsmrtsVector3( const PsmrtsVector3 &data, 
                              const int start_index, 
                              const int ndata ) :
                              PsmrtsBuffer<T>( data.buffer().slice( start_index, ndata ) ) {
        this->validate_state();
      }

      explicit PsmrtsVector3( const PsmrtsStridingBuffer &data ) :
                              PsmrtsBuffer<T>( data ) {
        this->validate_state();
      }


      /** User defined map to n_data T values where total_allocated() = ( n_data * stride_size_b )*/
      PsmrtsVector3( value_type *data, 
                     const int n_data,
                     const int stride_size_b = VectorBytes ) :
                     PsmrtsBuffer<T>( PsmrtsStridingBuffer( psmrts::cast_to_type<buffer_pointer, value_type *>( data ), 
                                                            stride_size_b, n_data ) ) {
        this->validate_state();
      }

      /** Destructor */
      virtual ~PsmrtsVector3() { }

      using PsmrtsBuffer<T>::size;
      using PsmrtsBuffer<T>::stride_size;
      using PsmrtsBuffer<T>::volume_size;
      using PsmrtsBuffer<T>::elapsed_life_time_s;
      using PsmrtsBuffer<T>::track_count;
      using PsmrtsBuffer<T>::performance_snapshot;      

      inline size_t vector_size() const { 
        return ( VectorSize );
      }

      inline const_vector_reference operator()( const int index = 0 ) const {
        return ( const_vector_reference( this->buffer_ptr( index ) ) );
      }

      inline vector_reference operator()( const int index = 0 ) {
        return ( vector_reference( this->buffer_ptr( index ) ) );
      }

      inline vector_type value( const int index = 0 ) const {
        return ( vector_type( this->buffer_ptr( index ) ) );
      }

      /** Extracts a 3-element vector and returns an Eigen::Vector3d  */
      inline Eigen::Vector3d vector3d( const int index = 0 ) const {
        const_pointer v = this->buffer_ptr( index );
        return ( Eigen::Vector3d( { v[0], v[1], v[2] } ) );
      }

      /** Extract a slice from the original dataset */
      inline PsmrtsVector3 slice( const int start_index, 
                                  const int n_data = 0 ) const {
        return ( PsmrtsVector3( this->buffer().slice( start_index, n_data ) ) );
      }

      /** Get a deep copy of the buffer */
      inline PsmrtsVector3 deep_copy( ) const {
        return ( PsmrtsVector3( this->buffer().deep_copy() ) );
      }

      using PsmrtsBuffer<T>::buffer;

    protected:

      using PsmrtsBuffer<T>::buffer_ptr;
      using PsmrtsBuffer<T>::buffer_ref;

    private:

      inline bool validate_state( const bool throwOnError = true ) const {
        bool isGood = true;
        if ( this->stride_size() < VectorBytes ) {
          if ( true == throwOnError ) {
            std::string mess = "Invalid stride size (" + std::to_string( this->stride_size() ) +
                              ") as PsmrtsVector3 requires (" + std::to_string( VectorBytes ) + ")";
            throw std::runtime_error( "PsmrtsVector3() - " + mess );
          }
          isGood = false;
        }

        // Check the stride buffer config
        bool isBufferValid = this->validate_stride_buffer( throwOnError );

        // Return status 
        return ( isGood && isBufferValid  );
      }
  };

  // Define a few useful types
  typedef PsmrtsVector3<double>   PsmrtsVector3d;
  typedef PsmrtsVector3<float>    PsmrtsVector3f;
  typedef PsmrtsVector3<int>      PsmrtsVector3i;


}  // namespace psmrts

#endif // PsmrtsVector3_hpp
