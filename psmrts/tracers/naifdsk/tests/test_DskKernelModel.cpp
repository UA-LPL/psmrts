
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

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
    
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size() == 0 ); // Should be zero, as we have yet to load any

    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); 
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 ); // should be 3, (Protected kernel, copy for dsk system, then copy of that)
    CHECK( dsk.use_count() == 3 );

    CHECK( dsk.isValid() == true );

    naif::DskKernelModel dsk2( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); 
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 );    
    CHECK ( naif::KernelFileSystem::size() == 1 );    
    CHECK( dsk2.use_count() == 4 );

    CHECK( dsk.handle()           == dsk2.handle() );
    CHECK( dsk.n_total_vertices() == dsk2.n_total_vertices() );
    CHECK( dsk.n_total_plates()   == dsk2.n_total_plates() );
    CHECK( dsk.n_dsk_segments()   == dsk2.n_dsk_segments() );

    
    int i = 1;
    auto kdscr = naif::KernelFileSystem::kernel_filetype_info("ALL");
    REQUIRE ( kdscr.size() == 1 );
    // CHECK ( kdscr[0].handle() == kdscr[1].handle() );
    for (const auto& element : kdscr) {
        CHECK ( element.m_handle == element.handle() );
        i++;
    }

    // DO NOT USE unload_kernel() to clean working inventory/pool - bypasses intended handling
    // and can cause multiple issues. Use: remove_dsk_shape() instead.
    CHECK_NOTHROW( naif::unload_kernel( dskfile ));
    CHECK ( naif::KernelFileSystem::size() == 1 );    

    auto kdscr2 = naif::KernelFileSystem::kernel_filetype_info("ALL");
    REQUIRE ( kdscr2.size() == 0 );
    i = 1;
    for (const auto& element : kdscr2) {
        CHECK ( element.m_handle == element.handle() );
        i++;
    }

    CHECK_NOTHROW( naif::KernelFileSystem::safe_disposal_of( dskfile ));
    // test unload kernel - get list of loaded, use internal functions to help
    // ensure count == 1 after - find out which one is unloaded / not valid


 
    // MAX loaded reference to kernels = 5300
    // do a try/catch that checks current load, and ensures the 5300 (AND/OR that any more throws an error)
    // #if 0: will not compile, but 1 will - to help debugging.
    int n_loaded = 0;
    bool done = false; 
    CHECK_NOTHROW ( naif::initKernelSystem( true ) );
    const bool ThrowOnErrorPlease = true;
    
    for ( ; ( n_loaded < 10000) && ( !done ) ; n_loaded++) {
        
        try {
            naif::load_kernel( dskfile );
            naif::check_naif_errors( ThrowOnErrorPlease );  
        }  
        catch(...) {
            done = true;
        } 
    }
    
    CHECK ( n_loaded == 5301 );

    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 );
    CHECK ( naif::KernelFileSystem::size() == 0 );

}


TEST_CASE ( "DSK Model Test - Multi-Load/Init/Shared Tests", "[kernel][dsk][shape][api]" ) {
    
    const double tolerance = 1.0e-9;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
 
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    // naif::check_naif_errors(); // Initializes the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size() == 0 );         // No cached files either...

    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors
    CHECK ( naif::KernelFileSystem::kernel_count() == 1 ); 
    CHECK ( naif::KernelFileSystem::size() == 1 ); 

    REQUIRE ( dsk.isValid() == true );
    REQUIRE ( dsk.n_dsk_segments() == 1 );
    CHECK ( dsk.shapefile() == dskfile );

    // ** Added tracer name checks **
    CHECK ( dsk.tracer_model_type() == "naifdsk" );
    CHECK ( dsk.tracer_model_name() == "DskKernelModel" );
    CHECK ( dsk.shape_tracer_id() == "naifdsk::DskKernelModel::" + dsk.shapefile());
 

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

    CHECK ( dsk.use_count() == 3 ); 

    naif::DskKernelModel dsk2;
    CHECK_THROWS ( dsk.create_from_id( 1 ) );
    CHECK_NOTHROW ( dsk2 = dsk.create_from_id( 2101955 ) );

    CHECK ( dsk2.use_count() == 4 ); 

    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::size() == 0 );

    
}


TEST_CASE ( "DSK Model Test - Dsk File API Tests", "[kernel][dsk][inventory][api]" ) {
    
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() );
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() );
    REQUIRE ( naif::KernelFileSystem::size() == 0 );  

    std::string dsk_test_file = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    naif::DskKernelModel dsk_test( dsk_test_file );

    // has_dsk_shape(file) - returns bool
    CHECK ( dsk_test.has_dsk_shape( dsk_test_file ) == true ); 

    // get_dsk_shape(file) - returns a DskKernelModel, use 
    naif::DskKernelModel dsk_test2 = dsk_test.get_dsk_shape( dsk_test_file );
    CHECK ( dsk_test2.shapefile() == dsk_test.shapefile());
    CHECK ( dsk_test.use_count() == 4 ); //tracks # of mutex locks, actual number being used

    // get_dsk_shape_with_id(file, id) - id = 2101955;
    naif::DskKernelModel dsk_test3 = dsk_test.get_dsk_shape_with_id( dsk_test_file, 2101955 );
    CHECK ( dsk_test3.shapefile() == dsk_test.shapefile() );
    CHECK ( dsk_test3.use_count() == 5 );

    // get_dsk_shape_inventory_list()  should be the file, and only one (should be same as size())
    auto dsk_inv = dsk_test.get_dsk_shape_inventory_list();
    CHECK ( dsk_inv.size() == 1 ); 

    // remove_dsk_shape() should removes from inventory
    dsk_test3.remove_dsk_shape( dsk_test_file );
    CHECK ( dsk_test.use_count() == 4 );
   

    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() );
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() );
}

TEST_CASE ("DSK Model Test - Ray Tracing / facet Routines", "[dsk][raytrace][facet]") {
    const double tolerance_r = 1.0e-6; // MM Precision

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
 
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size() == 0 );         // No cached files either...

    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() );
    naif::DskSegment segment = dsk.segment();
    CHECK_NOTHROW( naif::check_naif_errors() );

    CHECK ( dsk.use_count() == 3 ); 
    

    Eigen::Vector3d obs;
    double radius = segment.maximum_radius();
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;
    

    Eigen::Vector3d surf;
    double surf_long = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_long, surf_lat, surf.data() );
    // Attempt setting for sun, long 80 / lat 45 but with FAR distance (*100000).

    Eigen::Vector3d lkdr = surf - obs;
    psmrts::PsmrtsRayTrace ray(obs, lkdr);

    CHECK ( dsk.ray_trace(obs, lkdr, ray) == true );

    Eigen::Vector3d lkdr_norm = lkdr.normalized();
    Eigen::Vector3d sfpt_norm = ray.raypt().normalized();

    CHECK_THAT( lkdr_norm[0], Catch::Matchers::WithinAbs(sfpt_norm[0], tolerance_r ));
    CHECK_THAT( lkdr_norm[1], Catch::Matchers::WithinAbs(sfpt_norm[1], tolerance_r ));
    CHECK_THAT( lkdr_norm[2], Catch::Matchers::WithinAbs(sfpt_norm[2], tolerance_r ));

    double sep_ang = vsep_c(lkdr.data(), ray.raypt().data());
    CHECK_THAT( sep_ang, Catch::Matchers::WithinAbs(0.0, tolerance_r ));

    
    // load_facet_indexes ( segment ) / load_facet_vectors( segment ) 
    // Check individual facet extraction with indices and vector data
    naif::DskKernelModel::DskIndexDataModel indexes = dsk.load_facet_indexes();
    naif::DskKernelModel::DskVectorDataModel vectors = dsk.load_facet_vectors();
    
    CHECK ( dsk.n_total_plates()   == indexes.size() ); 
    CHECK ( dsk.n_total_vertices() == vectors.size() );

    psmrts::PsmrtsRayTrace::FacetDatum target_facet;
    psmrts::PsmrtsRayTrace  raytrace;
    
    raytrace.datum().m_hit = true;
    raytrace.datum().m_segment = segment.surfaceid(); 
    raytrace.datum().m_plateid = 1;

    naif::DskKernelModel::DskIndexDataModel::vector_type ones = naif::DskKernelModel::DskIndexDataModel::vector_type::Ones();
    for (int i = 0; i < dsk.n_total_plates(); i++) {
        // raytrace.m_plateid = i+1; 
        raytrace.datum().m_plateid = i; 
        dsk.get_facet( raytrace, target_facet );
        CHECK ( target_facet.isValid() == true ); 
        if ( target_facet.isValid() == false ) {
            CHECK ( i == -1 ); 
        }

        // Exporting of a NAIF DSK segment converts indexes to 0-based array references.
        // naif::DskKernelModel::DskIndexDataModel::vector_type indexes_plus_1 = indexes(i) - ones;
        naif::DskKernelModel::DskIndexDataModel::vector_type indexes_plus_1 = indexes(i);
        CHECK ( indexes_plus_1 == target_facet.m_indexes );  
        CHECK ( vectors( indexes(i)[0] ) == target_facet.m_vector1 );
        CHECK ( vectors( indexes(i)[1] ) == target_facet.m_vector2 );
        CHECK ( vectors( indexes(i)[2] ) == target_facet.m_vector3 );
    };

    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::size() == 0 );

}
