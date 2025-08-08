
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsShapeTracerAdapter.hpp>

#include <DskKernelModel.hpp>

// Test Default constructor for DskKernelModel
TEST_CASE ( "NAIF DSK Adapter Test", "[naif][shape][tracer][adapter]") {
    typedef psmrts::PsmrtsShapeTracerAdapter<naif::DskKernelModel>   NaifShapeTracer;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size()         == 0 ); // Should be zero, as we have yet to load any

    naif::DskKernelModel dsk( dskfile );
    std::shared_ptr<NaifShapeTracer> dsk_adapter(new NaifShapeTracer( dsk ) );
    CHECK_NOTHROW( naif::check_naif_errors() ); 
    REQUIRE( dsk_adapter.get() != nullptr );


    CHECK( dsk.handle()                == dsk.handle() );
    CHECK( dsk_adapter->vertex_count() == dsk.n_total_vertices() );
    CHECK( dsk_adapter->plate_count()  == dsk.n_total_plates() );
    CHECK_THAT( dsk_adapter->maximum_radius(), Catch::Matchers::WithinAbs( 0.2249388686, 1.0e-6 ) );

    CHECK( dsk_adapter->tracer_model_type()  == "naifdsk" );
    CHECK( dsk_adapter->tracer_model_name()  == "DskKernelModel" );
    CHECK( dsk_adapter->shape_tracer_id()    == dsk.shape_tracer_id() );
    CHECK( dsk_adapter->shapefile()          == dsk.shapefile() );

    Eigen::Vector3d obs;
    double radius = dsk_adapter->maximum_radius();
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    Eigen::Vector3d surf;
    double surf_long = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c( radius, surf_long, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = surf - obs;
    psmrts::PsmrtsRayTrace ray(obs, lkdr);

    CHECK ( dsk_adapter->ray_trace(obs, lkdr, ray) == true );

    std::shared_ptr<psmrts::PsmrtsTracerModel> dsk_adapter_clone( dsk_adapter->clone() );
    REQUIRE( dsk_adapter_clone.get() != nullptr );
    CHECK( dsk.use_count()           == 5 );

    std::shared_ptr<psmrts::PsmrtsTracerModel> dsk_ellipsoid( dsk_adapter->ellipsoid() );
    CHECK ( dsk_ellipsoid == nullptr ); // Placeholder Return, should return an ellipsoid model in future
    
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 );
    CHECK ( naif::KernelFileSystem::size()         == 0 );
}