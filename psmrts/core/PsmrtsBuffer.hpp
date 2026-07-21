#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsBuffer_hpp
#define PsmrtsBuffer_hpp

#include <cstddef>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsBufferData.hpp>
#include <psmrts/core/PsmrtsStridingBuffer.hpp>

namespace psmrts {
/**
 * @brief Provides template for generic type buffer and indexing
 *
 * This class provides the fundamentals of storage for a generic data.
 * It also provides a special feature in indexing that immediately
 * supports a striding buffer class or use it without the striding
 * buffer.

 * @author Kris J. Becker, University of Arizona
 * @history 2024-07-18 Kris J. Becker  Original Version
 */
template <typename T>
  class PsmrtsBuffer : public PsmrtsStridingBuffer {
    public:
      typedef T                           value_type;
      typedef value_type&                 reference;
      typedef const value_type&           const_reference;
      typedef value_type*                 pointer;
      typedef const value_type*           const_pointer;

      typedef PsmrtsStridingBuffer::value_type    buffer_type;
      typedef PsmrtsStridingBuffer::pointer       buffer_pointer;
      typedef PsmrtsStridingBuffer::const_pointer const_buffer_pointer;

      /** Default constructor */
      PsmrtsBuffer() : PsmrtsStridingBuffer() {
        init();
      }

      /** Construct an array of T values */
      PsmrtsBuffer( const int n_data ) {
        this->allocate_striding_buffer( n_data, sizeof( T ) );
      }

      /** Use to construct an array of T values spaced by stride size */
      PsmrtsBuffer( const int n_data,
                    const int striding_size_b ) {
        this->allocate_striding_buffer( n_data, striding_size_b );
      }

      /**
       * @brief Construct a slice from an existing PsmrtsBuffer object 
       * 
       * This constructor should be used to create a slice of an exising
       * PsmrtsBuffer. No data is copied and access to the original allocated
       * data is retained in a shared memory reference.
       * 
       * Note this constructor cannot be used to resize the stride size
       * of the existing buffer.
       * 
       * @param data 
       * @param starting_index 
       * @param ndata 
       */
      explicit PsmrtsBuffer( const PsmrtsBuffer &data, 
                             const int starting_index, 
                             const int ndata ) :
                             PsmrtsStridingBuffer( data.buffer().slice( starting_index, ndata ) ) {
      }

      /** Construct slice of an existing buffer */
      explicit PsmrtsBuffer( const PsmrtsStridingBuffer &data) :
                             PsmrtsStridingBuffer( data ) {
      }


      /** User defined map to n_data T values where total_allocated() = ( n_data * stride_size_b )*/
      PsmrtsBuffer( value_type *data, 
                    const int ndata,
                    const int stride_size_b = sizeof(T) ) :
                    PsmrtsStridingBuffer( psmrts::cast_to_type<buffer_pointer, pointer>( data ), 
                                          stride_size_b , ndata ) {
      }

      /** Destructor */
      virtual ~PsmrtsBuffer() { }

      using PsmrtsStridingBuffer::size;
      using PsmrtsStridingBuffer::stride_size;
      using PsmrtsStridingBuffer::volume_size;
      using PsmrtsStridingBuffer::data;
      using PsmrtsStridingBuffer::elapsed_life_time_s;
      using PsmrtsStridingBuffer::track_count;
      using PsmrtsStridingBuffer::performance_snapshot;

      /** Const reference to underlying striding buffer */
      inline const PsmrtsStridingBuffer &buffer() const {
        return ( *this );
      }

      inline const_reference operator()( const int index = 0 ) const {
        return ( this->buffer_ref( index ) );
      }

      inline reference operator()( const int index = 0 ) {
        return ( this->buffer_ref( index ) );
      }

      /** Extract a slice from the original dataset */
      inline PsmrtsBuffer slice( const int start_index, 
                                 const int n_data = 0 ) const {
        return ( PsmrtsBuffer( this->buffer().slice( start_index, n_data ) ) );
      }

      /** Get a deep copy of the buffer */
      inline PsmrtsBuffer deep_copy( ) const {
        return ( PsmrtsBuffer( this->buffer().deep_copy() ) );
      }

    protected:

      /** The non-const reference is under protected scope here */
      inline PsmrtsStridingBuffer &buffer() {
        return ( *this );
      }

      inline const_pointer get( const int index = 0 ) const {
        return ( this->buffer_ptr( index ) );
      }

      inline pointer get( const int index = 0 ) {
        return ( this->buffer_ptr( index ) );
      }

      inline pointer cast_ptr( buffer_pointer buffer_p ) {
        return ( reinterpret_cast<pointer> ( buffer_p ) );
      }

      inline const_pointer const_cast_ptr( const_buffer_pointer buffer_p ) const {
        return ( reinterpret_cast<const_pointer> ( buffer_p ) );
      }

      inline pointer buffer_ptr( const int index ) {
        return ( this->cast_ptr( this->buffer().get( index ) ) );
      }

      inline const_pointer buffer_ptr( const int index ) const {
        return ( this->const_cast_ptr( this->buffer().get( index ) ) );
      }

      inline reference buffer_ref( const int index ) {
        return ( *this->buffer_ptr( index ) );
      }

      inline const_reference buffer_ref( const int index ) const {
        return ( *this->buffer_ptr( index ) );
      }

  };

}  // namespace psmrts

#endif // PsmrtsBuffer_hpp
