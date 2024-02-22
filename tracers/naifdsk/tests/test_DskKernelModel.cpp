
#include <psmrts_catch2_environment.hpp>

#include <DskKernelModel.hpp>



// Test Default constructor for DskKernelModel
TEST_CASE ( "DSK Model Test - Default Constructor", "[default][dsk][shape]"){
    naif::DskSegment new_dsk;

    CHECK ( new_dsk.segment_number() == -1 );
    //CHECK ( new_dsk.dladsc().begin() == { 0 } );
    //CHECK ( new_dsk.dskdsc().begin() == { 0 } );
    CHECK ( new_dsk.n_vertices() == 0 ); 
    CHECK ( new_dsk.n_plates() == 0 );
    CHECK ( new_dsk.minimum_radius() == 0.0 );
    CHECK ( new_dsk.maximum_radius() == 0.0 );
    CHECK ( new_dsk.isValid() == false );
}

TEST_CASE ( "DSK Model Test - Basic Load/Init Tests", "[kernel][dsk][shape]" ) {
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    
    CHECK_NOTHROW ( naif::initKernelSystem() ); // Initializes the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any

    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); 
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 ); 
    CHECK( dsk.use_count() == 2 );

    CHECK( dsk.isValid() == true );

    naif::DskKernelModel dsk2( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); 
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 );    
    CHECK ( naif::KernelFileSystem::size() == 1 );    
    CHECK( dsk2.use_count() == 3 );

    CHECK( dsk.handle()           == dsk2.handle() );
    CHECK( dsk.n_total_vertices() == dsk2.n_total_vertices() );
    CHECK( dsk.n_total_plates()   == dsk2.n_total_plates() );
    CHECK( dsk.n_dsk_segments()   == dsk2.n_dsk_segments() );

    
    INFO ( "Kernels Before Unload: ");
    int i = 1;
    auto kdscr = naif::KernelFileSystem::kernel_filetype_info("ALL");
    REQUIRE ( kdscr.size() == 1 );
    // CHECK ( kdscr[0].handle() == kdscr[1].handle() );
    for (const auto& element : kdscr) {
        std::cout << "Kernel Before: " << i << ": " << element.m_handle << std::endl;
        CHECK ( element.m_handle == element.handle() );
        i++;
    }

    CHECK_NOTHROW( naif::unload_kernel( dskfile ));
    CHECK ( naif::KernelFileSystem::size() == 1 );    

    auto kdscr2 = naif::KernelFileSystem::kernel_filetype_info("ALL");
    REQUIRE ( kdscr2.size() == 0 );
    INFO ( "Kernels After: ");
    i = 1;
    for (const auto& element : kdscr2) {
        std::cout << "Kernel After: " << i << ": " << element.handle() << std::endl;
        CHECK ( element.m_handle == element.handle() );
        i++;
    }

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 );
    CHECK_NOTHROW( naif::KernelFileSystem::safe_disposal_of( dskfile ));
    // test unload kernel - get list of loaded, use internal functions to help
    // ensure count == 1 after - find out which one is unloaded / not valid
    // test loading same kernel multiple times. (2/19)

 
    // MAX loaded reference to kernels = 5300
    // do a try/catch that checks current load, and ensures the 5300 (AND/OR that any more throws an error)
    // #if 0: will not compile, but 1 will - to help debugging.
#if 0
    int n_loaded = 0;
    bool done = false; 
    CHECK_NOTHROW ( naif::initKernelSystem() );

    for ( ; ( n_loaded < 10000) && ( !done ) ; n_loaded++) {
        
        try {
            naif::load_kernel( dskfile );
            naif::check_naif_errors(false);  
        }  
        catch(...) {
            done = true;
        } 
    }
    
    CHECK ( n_loaded == 5301 );
    // kclear_c();
    CHECK_NOTHROW ( naif::initKernelSystem() );
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 );
#endif
    // Above test, when ran, seems to prevent the system from clearing the kernels. It gives error that KEEPER system
    // is maxed out, and persists through to other tests.
    // - Calling initKernelSystem(), clearKernelSystem(), and kclear_c() does not seem to work. 

}


TEST_CASE ( "DSK Model Test - Multi-Load/Init/Shared Tests", "[kernel][dsk][shape][api]" ) {
    
    const double tolerance = 1.0e-9;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
 
    CHECK_NOTHROW ( naif::initKernelSystem() );
    naif::check_naif_errors(); // Initializes the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any

    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 ); 

    REQUIRE ( dsk.isValid() == true );
    REQUIRE ( dsk.n_dsk_segments() == 1 );
    CHECK ( dsk.dskfile() == dskfile );
    // Since only one segment, should be same as below values for ie n_vertices(), n_plates(), etc

    // Test first segment of an open DSK kernel
    naif::DskSegment segment = dsk.segment();
    REQUIRE ( segment.isValid() == true ); // REQUIRE stops test if fail, CHECK continues after reporting result
    CHECK ( segment.segment_number() == 0 ); // Run test and replace with actual values
    CHECK ( segment.n_vertices() == 20 );
    CHECK ( segment.n_plates() == 36 );
    CHECK ( segment.dladsc_ptr() != nullptr ); // cept this
    CHECK ( segment.dskdsc_ptr() != nullptr ); // and this
    CHECK_THAT ( segment.minimum_radius(), Catch::Matchers::WithinAbs(0.2249388686, tolerance) );
    CHECK_THAT ( segment.maximum_radius(), Catch::Matchers::WithinAbs(0.283065 , tolerance) );
    CHECK ( segment.bodyid() == 2101955 ); // 2101955 (0x2012c3)
    CHECK ( segment.surfaceid() == 2101955 ); // 2101955 (0x2012c3)
    CHECK ( segment.frameid() == 10106 ); // 10106 (0x277a)
    CHECK ( segment.dtype() == 2 );
    CHECK ( segment.dclass() == 1 );

    CHECK ( dsk.n_total_vertices() == 20 );
    CHECK ( dsk.n_total_plates() == 36 );
    


    // Test DSK methods
    CHECK ( dsk.get_id_list()[0] == 2101955  );
    CHECK ( dsk.get_id_list().size() == 1 ); 

    CHECK ( dsk.get_segment_with_id( 2101955 )->segment_number() == segment.segment_number() ); // get_segment_with_id() returns memory address
    CHECK ( dsk.get_segment_with_id( 2101955 )->n_vertices() == segment.n_vertices() );
    CHECK ( dsk.get_segment_with_id( 2101955 )->n_plates() == segment.n_plates() );
    CHECK ( dsk.get_segment_with_id( 2101955 )->frameid() == segment.frameid() );
    CHECK ( dsk.get_segment_with_id( 12345 ) == nullptr );

    naif::DskKernelModel dsk2;
    CHECK_THROWS ( dsk.create_from_id( 1 ) );
    CHECK_NOTHROW ( dsk2 = dsk.create_from_id( 2101955 ) );


}