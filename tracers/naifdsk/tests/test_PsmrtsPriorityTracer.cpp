#define CATCH_CONFIG_MAIN
#include <psmrts_catch2_environment.hpp>

#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsDataModel.hpp>

#include <NaifUtilities.hpp>
#include <RayTrace.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsPriorityTracer.hpp>
#include <PsmrtsShapeTracerAdapter.hpp>
#include <NaifEllipsoidShape.hpp>

typedef psmrts::PsmrtsShapeTracerAdapter<naif::NaifEllipsoidShape> PsmrtsAdaptedEllipsoidShape;
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
    psmrts::RayTrace small_spt;
    psmrts::RayTrace large_spt;
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


    // wait for addition of .clear() function to clear out tracers, make sure 
    // original adapted shapes still have base values, then add a ray trace
    // that'll hit one but not the other, add them to a new tracer, and make
    // sure the one that'll get hit is returned by the priority function.
}