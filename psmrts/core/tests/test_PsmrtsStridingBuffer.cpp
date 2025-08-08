#include <psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsStridingBuffer.hpp>
#include <psmrts/core/PsmrtsBufferData.hpp>

/*
StridingBuffer allocates and assigns data as bytes, rather than hard data.
The default construction uses a base uint8_t as a type, with its' relative 
1 byte size (per inheritance structure from PsmrtsBufferData).

The distance function can help to determine number of bytes between dictated
indices. ie, get(n) and get(n+1) should always be equal to the allocated 
stride size.
*/

TEST_CASE( "PsmrtsStridingBuffer Default Test", "[striding][buffer][default]") {
    psmrts::PsmrtsStridingBuffer stride_buffer;

    CHECK( stride_buffer.size()          == 0 );
    CHECK( stride_buffer.stride_size()   == 1 ); // Value used in division elsewhere, cannot be 0
    CHECK( stride_buffer.volume_size()   == 0 );
    CHECK_THROWS( stride_buffer.get()    == nullptr ); 
    CHECK_THROWS( stride_buffer.ref()    == 0 );
    CHECK_THROWS( *stride_buffer.get(0)  == 0.0 );
    CHECK_THROWS( *stride_buffer.get(2)  == 0.0 );
    CHECK_THROWS( *stride_buffer.get(10) == 0.0 );
    CHECK_THROWS( stride_buffer.ref(0)   == 0.0 );
    CHECK_THROWS( stride_buffer.ref(5)   == 0.0 );
    CHECK_THROWS( stride_buffer.ref(10)  == 0.0 );
}

TEST_CASE( "PsmrtsStridingBuffer Values Test", "[striding][buffer][values]") {
    psmrts::PsmrtsStridingBuffer stride_buffer( 10, 4 );

    psmrts::PsmrtsStridingBuffer::pointer p = stride_buffer.get(0);
    for (int i = 0; i < stride_buffer.volume_size(); i++ ) {
        p[i] = i;
    }
    
    CHECK( stride_buffer.size()        == 10 );
    CHECK( stride_buffer.stride_size() == 4 );
    CHECK( stride_buffer.volume_size() == 40 );
    CHECK( stride_buffer.get(1)        != nullptr ); 
    CHECK( *stride_buffer.get(1)       == 4.0 ); 
    CHECK( &stride_buffer.ref(9)       == stride_buffer.get(9) );
    CHECK( stride_buffer.ref(9)        == *stride_buffer.get(9) ); // value = i * stride_size()

    for (int j = 0; j < stride_buffer.size(); j++ ) {
        CHECK( stride_buffer.ref(j) == j * stride_buffer.stride_size() );
    }
    for (int k = 0; k < stride_buffer.size(); k++ ) {
        CHECK( *stride_buffer.get(k) == k * stride_buffer.stride_size() );
    }

    CHECK( stride_buffer.validate_index(0) == true );
    CHECK_THROWS( stride_buffer.ref(11)    == 0.0 );
    CHECK_THROWS( stride_buffer.ref(41)    == 0.0 );
    CHECK_THROWS( stride_buffer.ref(-1)    == 0.0 );
    CHECK_THROWS( stride_buffer.get(45) );
    CHECK_THROWS( stride_buffer.get(35) ); 

    CHECK_THROWS( stride_buffer.validate_index(-1) == false );
    CHECK( stride_buffer.validate_index(-1, false) == false ); 

    psmrts::PsmrtsStridingBuffer stride_slice = stride_buffer.slice(4, 5);
    CHECK( stride_slice.size()        == 5 );
    CHECK( stride_slice.stride_size() == 4 );
    CHECK( stride_slice.volume_size() == 20 );
    CHECK( stride_slice.get(0)        != stride_buffer.get(0) );
    CHECK( stride_slice.get(0)        == stride_buffer.get(4) );
    CHECK( stride_slice.distance( stride_buffer.get(0), stride_slice.get(0)) == 16 ); // 4x4 strides

    psmrts::PsmrtsStridingBuffer stride_deep = stride_buffer.deep_copy();
    CHECK( &stride_buffer            != &stride_deep );
    CHECK( stride_deep.size()        == 10 );
    CHECK( stride_deep.stride_size() == 4 );
    CHECK( stride_deep.volume_size() == 40 );
    CHECK( stride_deep.get(1)        != nullptr ); 
    CHECK( *stride_deep.get(1)       == 4.0 ); 
    CHECK( &stride_deep.ref(9)       == stride_deep.get(9) );
    CHECK( stride_deep.validate_index(0) == true );

    // make_slice( start_index = 0, nstrides = 0 )
    psmrts::PsmrtsStridingBuffer slice_error;
    CHECK_THROWS( slice_error = stride_buffer.slice( -1, 5 ) );
    CHECK_THROWS( slice_error = stride_buffer.slice( 0, -1 ) );
    CHECK_NOTHROW( slice_error = stride_buffer.slice( 0, 0 ) );

    // validate_stride_buffer() fail check
    psmrts::PsmrtsBufferData pb_data;
    CHECK_THROWS( psmrts::PsmrtsStridingBuffer( pb_data, 1, 0 ));
}
