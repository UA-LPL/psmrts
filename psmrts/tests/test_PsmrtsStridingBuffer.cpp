#include <psmrts_catch2_environment.hpp>

#include <PsmrtsStridingBuffer.hpp>


// Just think of the data as bytes, not hard data types - otherwise assigning to individual bytes
// Focus on uint8 byte allocations
// Use the distance function to determine number of bytes between indexes
// index 0 to 9, get(n) and get(n+1) should always be the stride size and associative

TEST_CASE( "PsmrtsStridingBuffer Default Test", "[striding][buffer][default]") {
    psmrts::PsmrtsStridingBuffer stride_buffer;

    CHECK( stride_buffer.size() == 0 );
    CHECK( stride_buffer.stride_size() == 1 );
    CHECK( stride_buffer.volume_size() == 0 );
    CHECK_THROWS( stride_buffer.get() == nullptr ); 
    CHECK_THROWS( stride_buffer.ref() == 0 );
    CHECK_THROWS( *stride_buffer.get(0) == 0.0 );
    CHECK_THROWS( *stride_buffer.get(2) == 0.0 );
    CHECK_THROWS( *stride_buffer.get(10) == 0.0 );
    CHECK_THROWS( stride_buffer.ref(0) == 0.0 );
    CHECK_THROWS( stride_buffer.ref(5) == 0.0 );
    CHECK_THROWS( stride_buffer.ref(10) == 0.0 );
    // Make sure throws for all indexes, 0, 10 etc

}

TEST_CASE( "PsmrtsStridingBuffer Values Test", "[striding][buffer][values]") {
    psmrts::PsmrtsStridingBuffer stride_buffer( 10, 4 );

    psmrts::PsmrtsStridingBuffer::pointer p = stride_buffer.get(0);
    for (int i = 0; i < stride_buffer.volume_size(); i++ ) {
        p[i] = i;
    }
    
    CHECK( stride_buffer.size() == 10 );
    CHECK( stride_buffer.stride_size() == 4 );
    CHECK( stride_buffer.volume_size() == 40 );
    CHECK( stride_buffer.get(1) != nullptr ); 
    CHECK( *stride_buffer.get(1) == 4.0 ); 
    CHECK( &stride_buffer.ref(9) == stride_buffer.get(9) );
    CHECK( stride_buffer.ref(9) == *stride_buffer.get(9) ); // value = i * stride_size()
    for (int j = 0; j < stride_buffer.size(); j++ ) {
        CHECK( stride_buffer.ref(j) == j * stride_buffer.stride_size() );
    }
    for (int k = 0; k < stride_buffer.size(); k++ ) {
        CHECK( *stride_buffer.get(k) == k * stride_buffer.stride_size() );
    }
    CHECK( stride_buffer.validate_index(0) == true );
    CHECK_THROWS( stride_buffer.ref(11) == 0.0 );
    //CHECK_THROWS( stride_buffer.get(0)[45] == 45.0 ); // no throws, evals to 0
    //CHECK_THROWS( stride_buffer.get(0)[-1] == 39.0 ); // no throws, evals to 0
    CHECK_THROWS( stride_buffer.ref(41) == 0.0 );
    CHECK_THROWS( stride_buffer.ref(-1) == 0.0 );

    psmrts::PsmrtsStridingBuffer stride_slice = stride_buffer.slice(4, 5);
    CHECK( stride_slice.size() == 5 );
    CHECK( stride_slice.stride_size() == 4 );
    CHECK( stride_slice.volume_size() == 20 );
    CHECK( stride_slice.get(0) != stride_buffer.get(0) );
    CHECK( stride_slice.get(0) == stride_buffer.get(4) );
    CHECK( stride_slice.distance( stride_buffer.get(0), stride_slice.get(0)) == 16 ); // 4x4 strides

    psmrts::PsmrtsStridingBuffer stride_deep = stride_buffer.deep_copy();
    CHECK( &stride_buffer != &stride_deep );
    CHECK( stride_deep.size() == 10 );
    CHECK( stride_deep.stride_size() == 4 );
    CHECK( stride_deep.volume_size() == 40 );
    CHECK( stride_deep.get(1) != nullptr ); 
    CHECK( *stride_deep.get(1) == 4.0 ); 
    CHECK( &stride_deep.ref(9) == stride_deep.get(9) );
    CHECK( stride_deep.validate_index(0) == true );

}