#ifndef PsmrtsStridingBuffer_hpp
#define PsmrtsStridingBuffer_hpp

#include <cstddef>
#include <string>
#include <exception>
#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsBufferData.hpp>

namespace psmrts {

  /**
   * @brief PsmrtsStridingBuffer maps to a data buffer by a stridings size
   * 
   * This class is the fundamental class that provides an agnostic interface
   * to PSMRTS of variable data types. This pointer and reference offsets to
   * arbitrarily sized data structures. Note there is relation to any typed
   * data access so offsets and casts into actual data must be made by
   * derived classes.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2024-07-19 Kris J. Becker  Original Version
   */

  class PsmrtsStridingBuffer : public PsmrtsBufferData  {
    public:
      typedef PsmrtsBufferData::value_type           value_type;
      typedef PsmrtsBufferData::reference            reference;
      typedef PsmrtsBufferData::const_reference      const_reference;
      typedef PsmrtsBufferData::pointer              pointer;
      typedef PsmrtsBufferData::const_pointer        const_pointer;
      typedef PsmrtsBufferData::difference_type      difference_type;
      typedef PsmrtsBufferData::shared_pointer_type  shared_pointer_type;

      /** Default constructor */
      PsmrtsStridingBuffer() : PsmrtsBufferData(), 
                               m_stride_size_b( sizeof( value_type ) ),
                               m_size_s( 0 ) { }

      /** Construct an array of values */
      PsmrtsStridingBuffer( const int n_data,
                            const int stride_size_b ) {
        this->allocate_striding_buffer( n_data, stride_size_b );
      }

      /** Construct a buffer from a slice of a buffer data object */
      PsmrtsStridingBuffer( const PsmrtsBufferData &data,
                            const int stride_size_b,
                            const int ndata = 0 ) :
                            PsmrtsBufferData( data ), 
                            m_stride_size_b( stride_size_b ),
                            m_size_s( ndata ) {

        // If can contract. But can it expand if verified below deck?
        if ( 0 <= ndata ) {
          m_size_s = data.size() / stride_size_b;
        }

        // Check/validate configuration    
        this->validate_stride_buffer(); 

      }

      /** User defined map to n_data T values where total_allocated() = ( value_size() * size() )*/
      PsmrtsStridingBuffer( value_type *data, 
                            const int stride_size_b,
                            const int ndata ) :
                            PsmrtsBufferData ( data, ndata * stride_size_b ),
                            m_stride_size_b ( stride_size_b ),  
                            m_size_s( ndata ) {

        // Check/validate configuration    
        this->validate_stride_buffer();                               
      }


      /** Destructor */
      virtual ~PsmrtsStridingBuffer() { }

      /** Total number of strides allocated */
      inline size_t size() const {
        return ( m_size_s );
      }

      /** Total number of bytes in each stride - defaults to 1, value used in division elsewhere */
      inline size_t stride_size() const { 
        return ( m_stride_size_b );
      }

      /** Total number of bytes in buffer */
      inline size_t volume_size() const {
        return ( this->size() * this->stride_size() );
      }

      /** Returns a reference to buffer data  */
      inline const PsmrtsBufferData &data() const {
        return ( *this );
      }

      /** Returns const pointer to designated index position */
      inline const_pointer get( const int index = 0 ) const {
        return ( this->data_get( this->stride_data_offset( index ) ) );
      }

      /** Returns pointer to designated index poisition */
      inline pointer get( const int index = 0) {
        return ( this->data_get( this->stride_data_offset( index ) ) );
      }

      /** Returns const reference to designated index position */
      inline const_reference ref( const int index = 0 ) const {
        return ( this->data_ref( this->stride_data_offset( index ) ) );
      }

      /** Returns reference to designated index position */
      inline reference ref( const int index = 0) {
        return ( this->data_ref( this->stride_data_offset( index ) ) );
      }

      /**
       * @brief Provides a specific slice of the buffer
       * 
       * Returns a designated segment of the buffer as indicated by the 
       * start_index, representing the starting index of the desired 
       * slice location, and nstrides being the total number of data
       * values sized to the stride amount.
       * 
       * @param start_index             Start index for target slice
       * @param nstrides                Number of values to include in slice,
       *                                 sized to the stride value
       * @return PsmrtsStridingBuffer   Returns a PsmrtsStridingBuffer containing
       *                                 the slice as determined by above parameters
       */
      inline PsmrtsStridingBuffer slice( const int start_index, 
                                         const int nstrides = 0) 
                                         const {
        return ( make_slice( start_index, nstrides ) ); 
      }

      /** Get a deep copy of the buffer */
      inline PsmrtsStridingBuffer deep_copy( ) const {
        return ( PsmrtsStridingBuffer( this->data().deep_copy(), this->stride_size(), this->size() ) );
      }

      /** Compute distance of index to origin of the dataset in type indexes */
      inline difference_type distance( const_pointer base,
                                       const_pointer data_p ) const {
        return ( data_p - base );
      }

      /** Allow validation of an index with error control */
      inline bool validate_index( const int index, 
                                  const bool throwOnError = true ) const {
        
        int index_b = this->stride_data_offset( index );
        try {
          index_b = this->validate( index_b );
        }
        catch ( const std::runtime_error &re ) {
          if ( true == throwOnError ) throw;
          return ( false );
        }
        return ( true );
      }

      using PsmrtsBufferData::elapsed_life_time_s;
      using PsmrtsBufferData::track_count;
      using PsmrtsBufferData::performance_snapshot;

    protected:
    /** Special splice constructor method from this buffer */
      inline PsmrtsStridingBuffer make_slice( const int start_index = 0, 
                                              const int nstrides = 0 ) 
                                              const {
                                              
        if ( start_index < 0 ) {
          std::string mess = "Invalid starting index for slice (" + 
                             std::to_string( start_index ) + ")";
          throw std::runtime_error( "PsmrtsStrifingBuffer::make_slice()- " + mess );
        }

        if ( nstrides < 0 ) {
          std::string mess = "Invalid number of strides for slice (" + 
                             std::to_string( nstrides ) + ") must be >= 0";
          throw std::runtime_error( "PsmrtsStrifingBuffer::make_slice()- " + mess );
        }

        size_t offset_b  = this->stride_data_offset( start_index );
        size_t strides_t = nstrides;

        if ( strides_t <= 0 ) {
          strides_t = this->size( ) - start_index;
        }

        int n_bytes = strides_t * this->stride_size();
        return ( PsmrtsStridingBuffer( this->data().slice( offset_b, n_bytes ), this->stride_size(), strides_t ) );
      }

      /** The non-const reference is under protected scope here */
      inline PsmrtsBufferData &data() {
        return ( *this );
      }

      /** Calculates the designated stride offset */
      inline int stride_data_offset( const int index_t = 0 ) const {
        return ( index_t * this->stride_size() );
      }

      /**
       * @brief Validates the stride buffer configuration
       * 
       * This method can be called at any time to determine the state of the 
       * stride buffer configuration.
       * 
       * @param OnError 
       * @return true 
       * @return false 
       */
      inline bool validate_stride_buffer( const bool throwOnError = true ) const {

        bool isValid = false;
        try {

        // Ensure valid map
        size_t n_bytes = this->size() * this->stride_size();
        if ( n_bytes <= 0 ) {
          std::string mess = "Invalid stride size (" + std::to_string( this->stride_size() ) +
                             ") or count (" + std::to_string( this->size() ) +  ") for config of stride buffer";
          throw std::runtime_error( "PsmrtsStrideBuffer::validate_stride_buffer() - " + mess );
        }  

          this->validate( 0 );
          this->validate( n_bytes - 1 );
          this->validate( this->stride_data_offset( this->size() - 1 ) );
          isValid = true;
        }
        catch (const std::exception &e ) {
          if ( throwOnError == true )  throw;
          isValid = false;
        }

        return ( isValid );
      }

      /**
       * @brief Allocates the stride buffer configuration
       * 
       * This method is used to directly construct the striding buffer,
       * based on a provided size, and number, of strides. 
       * 
       * @param nstrides      Desired number of strides
       * @param stride_size_b Desired stride size, in bytes
       */
      inline void allocate_striding_buffer( const int nstrides,
                                            const int stride_size_b ) {

        // Sanity check, don't allow 0 bytes...
        int n_bytes = nstrides * stride_size_b;
        if ( n_bytes <= 0 ) {
          std::string mess = "Invalid stride size (" + std::to_string( stride_size_b ) +
                             ") or count (" + std::to_string( nstrides ) +  ") given for allocation of data";
          throw std::runtime_error( "PsmrtsStrideBuffer::allocate_striding_buffer() - " + mess );
        }                                              

        this->allocate( n_bytes );
        this->validate( n_bytes - 1 );

        m_stride_size_b = stride_size_b;
        m_size_s = nstrides;

        // Check/validate configuration    
        this->validate_stride_buffer();
      }

    private:
      size_t m_stride_size_b; // Number of data elements of stride size
      size_t m_size_s;        // Number of data elements of stride size

  };

}  // namespace psmrts

#endif // PsmrtsStridingBuffer_hpp
