

#include <psmrts_catch2_environment.hpp>

#include <NaifUtilities.hpp>


TEST_CASE ( "NAIF Utilities Test - Basics", "[naif][utilities]" ) {

    CHECK_NOTHROW( naif::initKernelSystem() );
    
}

TEST_CASE ( "NAIF Utilities Test - loading IK Kernal", "[naif][kernel]" ) {
    std::string file = psmrts_tracers_path( "/naifdsk/data/orx_ocams_v07.ti" );
    CHECK_NOTHROW ( naif::initKernelSystem() );
    CHECK_NOTHROW ( naif::load_kernel(file) );
    CHECK_NOTHROW ( naif::check_naif_errors() );
    CHECK_NOTHROW ( naif::unload_kernel(file) );
    CHECK_NOTHROW ( naif::check_naif_errors() );

    std::string bad_file = psmrts_tracers_path( "/naifdsk/data/orx_ocams_v07.ti.DNE" );
    CHECK_NOTHROW ( naif::initKernelSystem() );
    CHECK_NOTHROW ( naif::load_kernel(bad_file) );
    CHECK_THROWS ( naif::check_naif_errors() );

    // Replace load / unload with open / close functions for files in KernelFileSystem.hpp class
    // In KernelFileSystem:: - reference to methods, use size() to check for # of files in inventory

}

TEST_CASE ( "NAIF Utilities Test - isoc to ephemeris time ", "[naif][kernel][ephemeris]") {

    //const double tolerance = 1.0e-6;

    std::string file = psmrts_tracers_path( "/naifdsk/data/naif0012.tls" );

    CHECK_NOTHROW ( naif::initKernelSystem() );
    CHECK_NOTHROW ( naif::load_kernel(file) );
    CHECK_NOTHROW ( naif::check_naif_errors() );

    std::string time = "2024-02-02T15:30:00.000";


    CHECK_NOTHROW ( naif::isoc_to_et( time ) );

    double et = naif::isoc_to_et( time );
    std::string et_time = naif::et_to_isoc( et );

    CHECK ( time == et_time );


    CHECK_NOTHROW( naif::unload_kernel(file) );
    CHECK_NOTHROW( naif::check_naif_errors() );

}