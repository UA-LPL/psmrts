#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/algorithms/TracingBasics.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>

#include <cspice/SpiceUsr.h>

TEST_CASE("Tracing Basics - Base Ellipsoid Tracer Test", "[algorithms][tracing][basics][ellipsoid]") {
    const double tolerance_km = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0} );
    psmrts::EllipsoidTracer e_tracer( radii );

    Eigen::Vector3d obs;
    double obs_lon = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c( 1.0, obs_lon, obs_lat, obs.data() );
    obs = obs * 10.0;

    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c( 1.0, surf_lon, surf_lat, surf.data() );

    Eigen::Vector3d surf_obs = surf*1.5;
    psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs);

    bool result = psmrts::algorithms::process_basic_trace( e_tracer, prq_ray );

    CHECK( result == true );

}

TEST_CASE("Tracing Basics - Trace Array Test", "[algorithms][tracing][basics][array]") {
    const double tolerance_km = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0} );

    psmrts::EllipsoidTracer e_tracer( radii );

    // Ray Trace 1
    Eigen::Vector3d obs1;
    double radius1 = 1.0;
    double obs_long1 = 45.0 * rpd_c();
    double obs_lat1 = 45.0 * rpd_c();
    latrec_c ( radius1, obs_long1, obs_lat1, obs1.data() );
    obs1 = obs1 * 10.0;

    Eigen::Vector3d surf1;
    double surf_lon1 = 45.0 * rpd_c();
    double surf_lat1 = 50.0 * rpd_c();
    latrec_c ( radius1, surf_lon1, surf_lat1, surf1.data() );

    Eigen::Vector3d surf_obs1 = surf1 * 1.5;
    psmrts::PRQRayTrace prq_ray1(surf_obs1, -surf_obs1 );

    Eigen::Vector3d lookdir1 = prq_ray1.trace().xyz() - obs1;

    psmrts::PRQRayTrace prq_spt1( obs1, lookdir1 );

    // Ray Trace 2
    Eigen::Vector3d obs2;
    double radius2 = 1.0;
    double obs_long2 = 45.0 * rpd_c();
    double obs_lat2 = 45.0 * rpd_c();
    latrec_c ( radius2, obs_long2, obs_lat2, obs2.data() );
    obs2 = obs2 * 10.0;

    Eigen::Vector3d surf2;
    double surf_lon2 = 50.0 * rpd_c();
    double surf_lat2 = 45.0 * rpd_c();
    latrec_c ( radius2, surf_lon2, surf_lat2, surf2.data() );

    Eigen::Vector3d surf_obs2 = surf2 * 1.5;
    psmrts::PRQRayTrace prq_ray2(surf_obs2, -surf_obs2 );
 
    Eigen::Vector3d lookdir2 = prq_ray2.trace().xyz() - obs2;

    psmrts::PRQRayTrace prq_spt2(obs2, lookdir2 );

    // Ray Trace 3 (No hit condition)
    Eigen::Vector3d obs3;
    double radius3 = 1.0;
    double obs_long3 = 45.0 * rpd_c();
    double obs_lat3 = 45.0 * rpd_c();
    latrec_c ( radius3, obs_long3, obs_lat3, obs3.data() );
    obs3 = obs3 * 2.5;

    Eigen::Vector3d surf3;
    double surf_lon3 = 120.0 * rpd_c();
    double surf_lat3 = -45.0 * rpd_c();
    latrec_c ( radius3, surf_lon3, surf_lat3, surf3.data() );

    Eigen::Vector3d lookdir3 = surf3 - obs3; 

    psmrts::PRQRayTrace prq_spt3(obs3, lookdir3 );

    psmrts::PRQRayTraceArray ray_array;
    ray_array.add_trace(prq_spt3);
    bool result = psmrts::algorithms::process_basic_trace_array(e_tracer, ray_array);
    CHECK( result == true ); //should be false?

    bool result1 = psmrts::algorithms::process_basic_trace_array(e_tracer, ray_array);
    CHECK( result1 == true ); //should be false?

    ray_array.add_trace(prq_spt2);
    bool result2 = psmrts::algorithms::process_basic_trace_array(e_tracer, ray_array);
    CHECK( result2 == true );

    ray_array.add_trace(prq_spt1);
    bool result3 = psmrts::algorithms::process_basic_trace_array(e_tracer, ray_array);
    CHECK( result3 == true );
}

TEST_CASE("Tracing Basics - Trace Facet Test", "[algorithms][tracing][basics][facet]") {
    // WIP
}
