
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsBufferData.hpp>
#include <thread>
#include <chrono>

TEST_CASE( "PsmrtsBufferData Default Test", "[buffer][default]" ) {
    psmrts::PsmrtsBufferData pb_data;

    CHECK( pb_data.size()    == 0 );
    CHECK( pb_data.isValid() == false );
    CHECK( pb_data.elapsed_life_time_s() == 0.00e-10 );
}

TEST_CASE("PsmrtsBufferData Value Array Test", "[buffer][value][array]") {
    std::vector<psmrts::PsmrtsBufferData::value_type> my_vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    psmrts::PsmrtsBufferData pb_data( &my_vector[0], 10 );
    psmrts::PsmrtsThreadSafeCounter pb_performance = pb_data.performance_snapshot();

    CHECK( pb_data.size()       == 10 );
    CHECK( pb_data.isValid()    == true );
    CHECK( *pb_data.data_get(8) ==  8 ); 
    CHECK( pb_data.data_ref(1)  == 1 );
    CHECK( pb_data.data_origin_distance(7) == 7 );
    
    for (int i = 0; i < pb_data.size(); i++ ) {
        CHECK ( pb_data.data_ref(i) == my_vector[i] );
        CHECK ( pb_data.data_get(i) == &my_vector[i] ); 
    }


    psmrts::PsmrtsBufferData pb_slice = pb_data.slice(2, 4) ;
    CHECK( pb_slice.size()    == 4 ); 
    CHECK( pb_slice.isValid() == true );
    CHECK( pb_slice.data_origin_distance(1) == 1 );
    CHECK( pb_slice.distance( pb_data.data_get(0), pb_slice.data_get(0) ) == 2 ); 


    // Second param is # of values you want to slice, assumed to be contiguous    
    psmrts::PsmrtsBufferData pb_cb_slice ( pb_data.slice(2, 4) );
    CHECK( pb_cb_slice.size()    == 4 ); // Should be equal to second param above
    CHECK( pb_cb_slice.isValid() == true );
    CHECK( pb_cb_slice.data_origin_distance(1) == 1 );
    CHECK( pb_cb_slice.distance( pb_data.data_get(0), pb_cb_slice.data_get(0) ) == 2 ); 

    psmrts::PsmrtsBufferData pb_copy = pb_data.deep_copy();
    CHECK( &pb_copy != &pb_data );
    CHECK( pb_copy.size()        == pb_data.size() );
    CHECK( pb_copy.isValid()     == pb_data.isValid() );
    CHECK( pb_copy.track_count() == 0 ); 

    psmrts::PsmrtsBufferData pb_copy2 = pb_copy.deep_copy();
    CHECK( &pb_copy2 != &pb_copy );
    CHECK( pb_copy2.size()        == pb_data.size() );
    CHECK( pb_copy2.isValid()     == pb_data.isValid() );
    CHECK( pb_copy2.track_count() == 0 );
    CHECK( pb_copy.track_count()  == 0 ); 

    std::this_thread::sleep_for(std::chrono::seconds(1));
    CHECK( pb_data.elapsed_life_time_s() >= 1.0 );
    CHECK( pb_performance.runtime_s()    >= 1.0 );
    psmrts::PsmrtsThreadSafeCounter snapshot = pb_data.performance_snapshot();
    CHECK( snapshot.runtime_s()          >= 1.0 );
}
