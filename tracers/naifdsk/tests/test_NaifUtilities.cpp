

#include <psmrts_catch2_environment.hpp>

#include <NaifUtilities.hpp>


TEST_CASE ( "NAIF Utilities Test - Basics", "[naif][utilities]" ) {

    CHECK_NOTHROW( naif::initKernelSystem() );
    
}

TEST_CASE ( "NAIF Utilities Test - loading IK Kernal", "[naif][kernel]" ) {
    std::string file = "../tracers/naifdsk/data/orx_ocams_v07.ti";
    CHECK_NOTHROW ( naif::initKernelSystem() );
    CHECK_NOTHROW ( naif::load_kernel(file) );
    CHECK_NOTHROW ( naif::unload_kernel(file) );

    std::string bad_file = "../tracers/naifdsk/data/orx_ocams_v07.ti.DNE";
    CHECK_NOTHROW ( naif::initKernelSystem() );
    CHECK_THROWS ( naif::load_kernel(bad_file) );

}