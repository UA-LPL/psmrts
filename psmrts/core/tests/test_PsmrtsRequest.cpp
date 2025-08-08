#include <psmrts_catch2_environment.hpp>

#include <thread>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

TEST_CASE( "PsmrtsRequest Default Test", "[request][default]" ) {
    
    psmrts::PsmrtsRequest request_t;
    CHECK( request_t.name()           == "PsmrtsRequest" );
    CHECK( request_t.run_count()      == 0 );
    CHECK( request_t.process_status() == false );
    CHECK( request_t.was_invoked()    == false );
    CHECK( request_t.error_count()    == 0 );
    CHECK( request_t.max_error_cache_size() == 20 );

    request_t.process_running();
    CHECK( request_t.run_count()      == 1 );
    CHECK( request_t.process_status() == false );
    CHECK( request_t.was_invoked()    == true );

    request_t.process_running();
    CHECK( request_t.run_count()      == 2 );
    CHECK( request_t.process_status() == false );
    CHECK( request_t.was_invoked()    == true );

    request_t.process_complete();
    CHECK( request_t.run_count()      == 2);
    CHECK( request_t.process_status() == true );
    CHECK( request_t.was_invoked()    == true );

    CHECK( request_t.runtime_ms()     >=  0.0 );
    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ) ;
    CHECK_NOTHROW( request_t.process_complete( true ) );
    CHECK( request_t.runtime_ms()     >= 10.0 );

    request_t.add_error( std::runtime_error( "this is a error!") );
    CHECK( request_t.error_count()  == 1 );
    CHECK_THROWS( request_t.throw_errors() );
    request_t.clear_errors();
    CHECK( request_t.error_count()  == 0 );

    // Test caching of errors
    for ( size_t n = 0 ; n < ( request_t.max_error_cache_size() + 1 )  ; n++ ) {
     request_t.add_error( std::runtime_error( "Error number   " + std::to_string( n ) ) );
    }
    CHECK( request_t.error_count() == 20 );
}
