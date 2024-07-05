#include <psmrts_catch2_environment.hpp>

#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsDataModel.hpp>

#include <NaifUtilities.hpp>
#include <PsmrtsRayTrace.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsPriorityTracer.hpp>
#include <PsmrtsShapeTracerAdapter.hpp>
#include <NaifEllipsoidShape.hpp>
#include <DskKernelModel.hpp>

typedef psmrts::PsmrtsShapeTracerAdapter<naif::NaifEllipsoidShape> PsmrtsAdaptedEllipsoidShape;
typedef psmrts::PsmrtsShapeTracerAdapter<naif::DskKernelModel>   NaifShapeTracer;
typedef std::shared_ptr<psmrts::PsmrtsTracerModel>  SharedTracerModel;

TEST_CASE( "Naif Priority Tracer Default Test", "[priority][tracer][default]") {
    const double tolerance = 1.0e-6;

    naif::NaifEllipsoidShape s_ellipse( 1.0, 2.0, 3.0, "small" );
    SharedTracerModel small_ellipsoid(new PsmrtsAdaptedEllipsoidShape( s_ellipse ));
    CHECK ( s_ellipse.shapefile() == "small" );
    CHECK ( small_ellipsoid->shapefile() ==  "small" );
    

    naif::NaifEllipsoidShape l_ellipse( 10.0, 20.0, 30.0, "large" );
    SharedTracerModel large_ellipsoid(new PsmrtsAdaptedEllipsoidShape( l_ellipse ));
    CHECK ( l_ellipse.shapefile() == "large" );
    CHECK ( large_ellipsoid->shapefile() == "large" );
   

    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    Eigen::Vector3d surf;
    double surf_long = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_long, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = surf - obs;
    psmrts::PsmrtsRayTrace small_spt;
    psmrts::PsmrtsRayTrace large_spt;
    REQUIRE ( small_ellipsoid->ray_trace(obs, lkdr, small_spt) == true );
    REQUIRE ( large_ellipsoid->ray_trace(obs, lkdr, large_spt) == true );

    psmrts::PsmrtsPriorityTracer test_tracers;
    CHECK ( test_tracers.isValid() == false );

    test_tracers.add_tracer( small_ellipsoid );
    test_tracers.add_tracer( large_ellipsoid );
    
    CHECK ( test_tracers.isValid() == true );
    CHECK ( test_tracers.size() == 2 );
    
    auto small_ptr = test_tracers.find_model_by_name("small");
    REQUIRE( small_ptr != nullptr );
    CHECK ( small_ptr->shapefile() == "small" );

    auto large_ptr = test_tracers.find_model_by_name("large");
    REQUIRE( large_ptr != nullptr );
    CHECK ( large_ptr->shapefile() == "large" );


    auto small_id_ptr = test_tracers.find_model_by_id( "psmrts::NaifEllipsoid::small" );
    REQUIRE ( small_id_ptr != nullptr );
    CHECK ( small_id_ptr->shapefile() == "small" );

    auto large_id_ptr = test_tracers.find_model_by_id( "psmrts::NaifEllipsoid::large" );
    REQUIRE ( large_id_ptr != nullptr );
    CHECK ( large_id_ptr->shapefile() == "large" );


    std::vector<std::string> shape_list = test_tracers.get_shapefile_names("psmrts", "NaifEllipsoid"); 
    CHECK ( shape_list.size() == 2 );
    CHECK ( shape_list[0] == "small" );
    CHECK ( shape_list[1] == "large" );

    test_tracers.clear();
    REQUIRE( test_tracers.size() == 0 );

    CHECK ( small_ellipsoid->shape_tracer_id() == "psmrts::NaifEllipsoid::small" );
    CHECK ( large_ellipsoid->shape_tracer_id() == "psmrts::NaifEllipsoid::large" ); 
    
    // add a ray trace
    // that'll hit one but not the other, add them to a new tracer, and make
    // sure the one that'll get hit is returned by the priority function.

    // 
}

TEST_CASE( "Priority Tracer Ray Trace Test", "[priority][tracer][dsk][naif]") {
    // Objects added to Tracer
    naif::NaifEllipsoidShape s_ellipse( 0.1 ); //Small body radius: 0.1 KM
    SharedTracerModel small_ellipsoid( new PsmrtsAdaptedEllipsoidShape( s_ellipse ));

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" ); // Avg Radius: 0.25 KM
    naif::DskKernelModel dsk( dskfile );
    std::shared_ptr<NaifShapeTracer> dsk_adaptor( new NaifShapeTracer( dsk ) ); 

    Eigen::Vector3d obs = {0.0, 10.0, 0.0};
    /*
    double radius = 0.1;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;   
    */
    Eigen::Vector3d lkdr = -obs + Eigen::Vector3d({0.0, 0.0, 0.10});
    
    double lat, lon, rad;
    reclat_c(obs.data(), &rad, &lon, &lat); 
    lon = lon * dpr_c(); 
    lat = lat * dpr_c();
    CHECK ( rad == 10 );
    CHECK ( lon == 90 );
    CHECK ( lat == 0 );


    // Tracer settup
    psmrts::PsmrtsPriorityTracer test_tracers;
    test_tracers.add_tracer( small_ellipsoid );
    test_tracers.add_tracer( dsk_adaptor );

    psmrts::PsmrtsRayTrace ray;

    auto shape_t = test_tracers.ray_trace(obs, lkdr, ray);
    REQUIRE ( ray.hasHit() == true );
    REQUIRE ( shape_t != nullptr );
    CHECK ( shape_t->shapefile() == small_ellipsoid->shapefile() );
    
    // loop through cases, if z value in lkdr for any values greater than ellipsoid radius (0.1)
    // should return ptr at bennu facet.
    Eigen::Vector3d observer;
    double radius = 0.1;
    double obs_long = 90.0 * rpd_c();// Direct View
    double obs_lat = 0.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, observer.data() );
    observer = observer * 10.0;  
    
    std::vector<double> z_list = {0.05, 0.08, 0.10, 0.11, 0.12, 0.15, 0.20};

    psmrts::PsmrtsPriorityTracer next_tracer;
    next_tracer.add_tracer( small_ellipsoid );
    next_tracer.add_tracer( dsk_adaptor );

    for ( auto z_value: z_list ) {
        Eigen::Vector3d lkdr = -observer + Eigen::Vector3d({0.0, 0.0, z_value});

        psmrts::PsmrtsRayTrace raytrace;

        auto shape_trace = next_tracer.ray_trace(observer, lkdr, raytrace);
        REQUIRE( raytrace.hasHit() == true);
        REQUIRE( shape_trace != nullptr);
        
        // Anything larger than the ellipse (Radius 0.10), should return the 
        // DSK file (Avg. 0.250 radius). Any values outside of ones tested 
        // above (Long, Lat values, and radii) will result in failed tests 
        // unless previously computed correctly.
        if (z_value > 0.10) { // at these values, goes to 0.11 z value before favoring larger body
            CHECK (shape_trace->shapefile() == dsk_adaptor->shapefile()); 
        }
        else {
            CHECK (shape_trace->shapefile() == small_ellipsoid->shapefile());
        }
    }

    next_tracer.clear();
    CHECK ( next_tracer.size() == 0 );
    next_tracer.add_tracer( dsk_adaptor );
    next_tracer.add_tracer( small_ellipsoid );
    CHECK ( next_tracer.size() == 2 );

    for ( auto const &z_value: z_list ) {
        Eigen::Vector3d lkdr = -observer + Eigen::Vector3d({0.0, 0.0, z_value});

        psmrts::PsmrtsRayTrace raytrace;

        auto shape_trace = next_tracer.ray_trace(observer, lkdr, raytrace);
        REQUIRE( raytrace.hasHit() == true);
        REQUIRE( shape_trace != nullptr);
        // Order of adding the tracer matters, should always intercept the dsk in this case.
        CHECK (shape_trace->shapefile() == dsk_adaptor->shapefile()); 

    }

    next_tracer.clear();
    REQUIRE ( next_tracer.size() == 0);

    // Check for empty tracer - no priority
    for ( auto const &z_value: z_list ) {
        Eigen::Vector3d lkdr = -observer + Eigen::Vector3d({0.0, 0.0, z_value});

        psmrts::PsmrtsRayTrace raytrace;

        auto shape_trace = next_tracer.ray_trace(observer, lkdr, raytrace);
        REQUIRE( raytrace.hasHit() == false);
        REQUIRE( shape_trace == nullptr);

    }
    
}