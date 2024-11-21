#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsRequest.hpp>

TEST_CASE( "PsmrtsRequest Default Test", "[request][default]" ) {
    
    psmrts::PsmrtsRequest request_t;
    CHECK( request_t.name()        == "PsmrtsRequest" );
    CHECK( request_t.run_count()   == 0 );
    CHECK( request_t.was_invoked() == false );
    CHECK( request_t.error_count()  == 0 );

    request_t.process_running();
    CHECK( request_t.run_count()   == 1 );
    CHECK( request_t.was_invoked() == false );

    request_t.process_running();
    CHECK( request_t.run_count()   == 2 );
    CHECK( request_t.was_invoked() == false );

    request_t.process_complete();
    CHECK( request_t.run_count()   == 2);
    CHECK( request_t.was_invoked() == true );

    request_t.add_error( std::runtime_error( "this is a error!") );
    CHECK( request_t.error_count()  == 1 );
    CHECK_THROWS( request_t.throw_errors() );
    request_t.clear_errors();
    CHECK( request_t.error_count()  == 0 );
}


int myfunc( double x ) { return 1; }
int myfunc( int x ) { return 2; }
int myfunc( ) { return 2; }


TEST_CASE( "PsmrtsRequest Callable Test", "[request][callable][traits]" ) {
    

    struct MyProcess {

        size_t size( double  x ) const { return 1; }
        bool process( int v )  {
            return ( true );
        }
#if 0
        bool process( std::string v ) const {
            return ( true );
        }        
#endif        
    };

    CHECK( psmrts::traits::has_process_method< MyProcess, int>  == true );

    // make
    //CHECK( psmrts::traits::is_process_callable< MyProcess, int>  == true );

}