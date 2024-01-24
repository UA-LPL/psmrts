

#include <psmrts_catch2_environment.hpp>

#include <NaifUtilities.hpp>


TEST_CASE ( "NAIF Utilities Test - Basics", "[naif][utilities]" ) {

    CHECK_NOTHROW( naif::initKernelSystem() );
    
}