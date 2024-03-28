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
    

    naif::NaifEllipsoidShape l_ellipse( 10.0, 20.0, 30.0, "large" );
    SharedTracerModel large_ellipsoid(new PsmrtsAdaptedEllipsoidShape( l_ellipse ));
   

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
    test_tracers.add_tracer( small_ellipsoid );
    test_tracers.add_tracer( large_ellipsoid );
    
    
    
}