#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsBufferData_hpp
#define PsmrtsBufferData_hpp

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>

#include <psmrts/core/PsmrtsUtilities.hpp>
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
      PsmrtsBufferData( const int n_data_b ) {
        init();
        allocate( n_data_b );
        validate( n_data_b - 1 );
      }

      /** Construct an array of values */
      PsmrtsBufferData( const shared_pointer_type &data,
                        const int n_data_b ) :
                        m_data( data ), 
                        m_data_ptr( data.get() ),
                        m_size( n_data_b ),
                        m_tracker() {
        validate( n_data_b - 1 );
      }


      /** User defined map to n_data T values where total_allocated() = ( value_size() * size() )*/
      PsmrtsBufferData( value_type *data, const int n_data ) {
        init();
        m_data_ptr = data;
        m_size     = n_data;
        validate( n_data - 1 );
      }

      /** Construct a safe slice with no new memory allocation into original data buffer */
      PsmrtsBufferData( const PsmrtsBufferData &data, 
                        const int start_index = 0, 
                        const int n_data = 0 ) {

        init();                          
                            
        if ( data.isValid() ) {

          m_data     = data.m_data;
          m_data_ptr = data.m_data_ptr + start_index;
        
          // Determine the number of values to map
          int n_values = n_data;

          // Occurs on a simple copy
          if ( 0 == n_values ) n_values = data.size() - start_index;

          // Now update slice size
          m_size = n_values;

          // Validate the configuration
          try {
            data.validate( start_index );
            data.validate( start_index + n_values - 1 );

            this->validate( 0 );
            this->validate( this->size() - 1 );
          }
          catch ( const std::runtime_error &e ) {
            std::string msg = "Invalid segment into data slice!\n" + std::string( e.what() );
            throw std::runtime_error( "PsmrtsBufferData::PsmrtsBufferData() - " + msg );
          }
        }
      }

      /** Destructor */
      virtual ~PsmrtsBufferData() { }

      /** Total number of data T allocated */
      inline size_t size() const {
        return ( m_size );
      }

      /** Returns validity state of data buffer */
      inline bool isValid() const {
        return ( nullptr != m_data_ptr );
      }

      /** Returns pointer to a parameter-designated index target in the data buffer */
      inline pointer data_get( const int index = 0 ) {
        return ( m_data_ptr + this->validate_index( index ) );
      }

      /** Returns a const pointer to a parameter-designated index target in the data buffer */
      inline const_pointer data_get( const int index = 0 ) const {
        return ( m_data_ptr + this->validate_index( index ) );
      }

      /** Returns a copy of the T value at the given index */
      inline reference data_ref( const int index )  {
        return ( m_data_ptr[this->validate_index( index )] );
      }

      /** Returns a copy of the T value at the given index */
      inline const_reference data_ref( const int index ) const {
        return ( m_data_ptr[this->validate_index( index )] );
      }

      /** Extract a slice from the original dataset */
      inline PsmrtsBufferData slice( const int start_index, 
                                     const int n_data = 0 ) const {
        return ( PsmrtsBufferData( *this, start_index, n_data ) );
      }

      /** Get a deep copy of the buffer */
      inline PsmrtsBufferData deep_copy( ) const {
        PsmrtsBufferData copy_t( this->size() );
        for ( size_t n = 0 ; n < this->size() ; n++ ) {
          copy_t.data_ref( n ) = this->data_ref( n );
        }
        return ( copy_t );
      }
    
      /** Compute distance of index to origin of the dataset in type indexes */
      inline difference_type data_origin_distance( const int index ) const {
        const_pointer origin = m_data_ptr;   //  ( nullptr != m_data.get() ) ? m_data.get() : m_data_ptr ;  ??
        return ( this->distance( origin, this->data_get( index ) ) );
      }

      /** Compute distance of index to origin of the dataset in type indexes */
      inline difference_type distance( const_pointer base,
                                       const_pointer data_p ) const {
        return ( data_p - base );
      }

      /** Validate the index into a T value */
      inline int validate( const int index ) const {
        if ( (index < 0 ) || ( index >= m_size ) ) {
          std::string mess = "Invalid index (" + std::to_string( index ) +
                              "), valid index range is (0, " + std::to_string( m_size ) + " - 1)";
          throw std::runtime_error( mess );
        }
        return ( index );        
      }

      inline double elapsed_life_time_s() const {
        return ( m_tracker.runtime_s() );
      }

      inline size_t track_count() const {
        return ( m_tracker.count() );
      }

      /**
       * @brief Return a standalone clone of the current tracker stats
       *  
       * Get a snapshot of the performance at this moment. I'd immediately get
       * an end_time = system_clock_time
       * 
       * @return PsmrtsThreadSafeCounter 
       */
      inline PsmrtsThreadSafeCounter performance_snapshot() const {
        return ( m_tracker.clone() );
      }

    protected:
      ///! Define the desired base type allocation unit type
      ///!  for alignment considerations
      typedef  uint16_t      BaseAllocationUnit;

      /** Validate the index into a T value */
      inline int validate_index( const int index ) const {
#if defined(PSMRTS_BOUNDS_CHECK) 
        return ( this->validate( index ) ); 
#else       
        return ( index );        
#endif
      }


      /** Reset all variables to default state which releases any prior data */
      inline void init() {

        m_data.reset();
        m_data_ptr  = m_data.get();
        m_size      = 0;
        m_tracker = PsmrtsThreadSafeCounter();

        return;
      }

      /** Allocate n_data byte elements */
      inline void allocate( const int n_data_b ) {
        try {
          size_t v_alloc = n_data_b;

          // Allocate using a base type/size allocation unit for alignment considerations
          size_t b_alloc = ( v_alloc + ( sizeof( BaseAllocationUnit ) - 1 ) ) / sizeof( BaseAllocationUnit );
          std::shared_ptr<BaseAllocationUnit> b_data( new BaseAllocationUnit[b_alloc], 
                                                      std::default_delete<BaseAllocationUnit[]>() );

          // Initialize object
          m_data      = psmrts::cast_shared_ptr<value_type, BaseAllocationUnit>( b_data );
          m_data_ptr  = m_data.get();
          m_size      = n_data_b;
        }
        catch ( const std::bad_alloc &b_alloc ) {
          init();
          std::string msg = "Failed to allocate data buffer of size " +
                            std::to_string( n_data_b );
          throw std::runtime_error( msg + "\n" + b_alloc.what() );
        }

        return;
      }

    private:
      std::shared_ptr<value_type> m_data;        // Data array T scalar values
      pointer                     m_data_ptr;    // This will allow for user
                                                 //   defined data access
      size_t                      m_size;        // Number of values of T
      PsmrtsThreadSafeCounter     m_tracker;     // Tracks times and copy counts

  };

}  // namespace psmrts

#endif // PsmrtsBufferData_hpp
