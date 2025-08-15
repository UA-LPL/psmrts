
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/tracers/ellipsoid/private/EllipsoidTracerModel.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>

TEST_CASE( "EllipsoidTracerModel Default Test", "[ellipsoid][default]") {
    
    psmrts::EllipsoidTracerModel e_shape;

    // No Parameter Initialization
    CHECK ( e_shape.name() == "ellipsoid" );

    CHECK ( e_shape.maximum_radius() == 0.0 ); 

    // Creating Tracer Model from Naif Ellipsoid
    psmrts::EllipsoidTracer naif_ellipse( 1.0, 2.0, 3.0 );

    psmrts::EllipsoidTracerModel etm_ellipse ( 1.0, 2.0, 3.0  );

    CHECK ( etm_ellipse.name() == "ellipsoid" );
    // CHECK ( etm_ellipse.maximum_radius() == naif_ellipse.c() );

    // Creating Tracer Model from Eigen Radii
    Eigen::Vector3d e_radii( {1.0, 1.0, 2.0 } );

    psmrts::EllipsoidTracerModel etm_radii_ellipse ( e_radii, "my_sphere" );

    CHECK ( etm_radii_ellipse.name() == "my_sphere" );
    CHECK ( etm_radii_ellipse.maximum_radius() == 2.0 );

    // Deliberate Empty Name Check and Clone
    psmrts::EllipsoidTracerModel etm_NoName (e_radii, "");
    // CHECK_NOTHROW( etm_NoName.performance_snapshot() );

}

TEST_CASE ("EllipsoidTracerModel Basic Values / RayTrace Test", "[tracer][ellipsoid][raytrace]") {

    const double tolerance = 1.0e-6;

    psmrts::EllipsoidTracerModel naif_ellipse(1.0, 2.0, 3.0);

    psmrts::EllipsoidTracer etm_ellipse ( 1.0, 2.0, 3.0 );

    Eigen::Vector3d observer;
    double radius = naif_ellipse.maximum_radius();
    CHECK( radius == 3.0 );
    double obs_long = psmrts::degrees_to_radians(45.0); 
    double obs_lat = psmrts::degrees_to_radians(45.0);
    latrec_c ( radius, obs_long, obs_lat, observer.data() );
    observer = observer * 2.0;

    Eigen::Vector3d surf;
    double surf_long = psmrts::degrees_to_radians(45.0);
    double surf_lat = psmrts::degrees_to_radians(50.0); 
    latrec_c ( radius, surf_long, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = surf - observer;

    psmrts::PsmrtsRayTrace naif_ray;
    psmrts::PsmrtsRayTrace etm_ray;

    CHECK ( naif_ellipse.ray_trace( observer.data(), lkdr.data(), naif_ray.datum().m_xyz.data(), naif_ray.datum().m_normal.data() ) == true );
    CHECK ( etm_ellipse.ray_trace( observer, lkdr, etm_ray ) == true );
}
