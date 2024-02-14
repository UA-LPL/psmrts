
#include <psmrts_catch2_environment.hpp>

#include <DskKernelModel.hpp>


TEST_CASE ( "DSK Model Test - Basic Load/Init Tests", "[kernel][dsk][shape]" ) {
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    
    CHECK_NOTHROW ( naif::initKernelSystem() ); // Initializes the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any

    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 ); // Should be zero, as we have yet to load any

    CHECK( dsk.isValid() == true );

    naif::DskKernelModel dsk2( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors
    CHECK ( naif::KernelFileSystem::kernel_count() == 2 ); // Should be zero, as we have yet to load any    
}
