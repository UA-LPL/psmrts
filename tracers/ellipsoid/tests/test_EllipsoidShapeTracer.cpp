#include <psmrts_catch2_environment.hpp>

#include <EllipsoidShapeTracer.hpp>
#include <EllipsoidTracerModel.hpp>
#include <PsmrtsUtilities.hpp>

TEST_CASE( "Ellipsoid Shape Tracer - Request Default Constructor", "[default][ellipsoid][shapetracer]") {
    psmrts::EllipsoidShapeTracer e_tracer;

    psmrts::PRQFeatures features;
    CHECK( e_tracer.process( features ) == true );

    CHECK( features.to_string() == "[{\"name\":\"ellisoid\",\"product\":\"shapetracer\",\"mesh\":false,\"radii\":[1,2,3]}]" ); 
    CHECK( features.config().dump() == "[{\"name\":\"ellisoid\",\"product\":\"shapetracer\",\"mesh\":false,\"radii\":[1,2,3]}]");
    // feature-specific functions testing in bullet version
}

TEST_CASE( "Ellipsoid Shape Tracer Test", "[ellipsoid][shapetracer]") {
    const double tolerance_km = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0} );

    psmrts::EllipsoidShapeTracer e_tracer( radii );

    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    Eigen::Vector3d surf_obs = surf*1.5;
    psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs );
    REQUIRE( e_tracer.process( prq_ray ) == true ); 

    Eigen::Vector3d lkdr = prq_ray.trace().xyz() - obs;

    psmrts::PRQRayTrace prq_spt(obs, lkdr );
    REQUIRE( e_tracer.process( prq_spt) );

    Eigen::Vector3d normal = prq_spt.trace().normal();
    Eigen::Vector3d xyz = prq_spt.trace().xyz();

    CHECK( prq_ray.isValid() == true );
    CHECK( prq_spt.isValid() == prq_spt.trace().hasHit() );

    CHECK_THAT( normal[0], Catch::Matchers::WithinAbs( 0.45451947767204381, tolerance_km ));
    CHECK_THAT( normal[1], Catch::Matchers::WithinAbs( 0.4545194776720437, tolerance_km )); 
    CHECK_THAT( normal[2], Catch::Matchers::WithinAbs( 0.7660444431189779, tolerance_km ));

    double et_lat, et_lon, et_radius;
    reclat_c(xyz.data(), &et_radius, &et_lon, &et_lat);

    CHECK_THAT( et_lon, Catch::Matchers::WithinAbs( 45.0 * rpd_c(), tolerance_km));
    CHECK_THAT( et_lat, Catch::Matchers::WithinAbs( 50.0 * rpd_c(), tolerance_km));

    CHECK_THAT( et_radius, Catch::Matchers::WithinAbs( prq_spt.trace().radius(), tolerance_km ));
    CHECK_THAT( et_lon, Catch::Matchers::WithinAbs( surf_lon, tolerance_km));
    CHECK_THAT( et_lat, Catch::Matchers::WithinAbs( surf_lat, tolerance_km));

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[0], tolerance_km));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[1], tolerance_km));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[2], tolerance_km));
}

TEST_CASE("Ellipsoid Shape Tracer Photometric Values Test", "[ellipsoid][shapetracer][photometric]") {
    const double tolerance = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0 } );
    psmrts::EllipsoidShapeTracer e_tracer( radii );

    // Compute the position of the observer at (45, 45, 10)
    Eigen::Vector3d observer;
    double radius = 1.0;
    double obs_long = psmrts::degrees_to_radians( 45.0 );
    double obs_lat = psmrts::degrees_to_radians( 45.0 );
    latrec_c( radius, obs_long, obs_lat, observer.data() );
    observer = observer * 10.0;

    // Compute the surface vector at (45, 50, 1). This is our surface target vector
    Eigen::Vector3d surf;
    double surf_lon = psmrts::degrees_to_radians( 45.0 );
    double surf_lat = psmrts::degrees_to_radians( 50.0 );
    latrec_c( radius, surf_lon, surf_lat, surf.data() );

    // Calculate real surface point 
    Eigen::Vector3d surf_obs = surf * 1.5; 
    psmrts::PRQRayTrace prq_surf( surf_obs, -surf_obs );
    CHECK( e_tracer.process( prq_surf ) == true );
    CHECK( surf_obs == prq_surf.trace().observer() );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lookdir = prq_surf.trace().xyz() - observer;

    // Create trace from observer to surface xyz = ( 45, 50, r km )
    psmrts::PRQRayTrace prq_ray( observer, lookdir );
    CHECK( e_tracer.process( prq_ray ) == true );

    // Rigorous check of surface points
    Eigen::Vector3d ps_xyz = prq_surf.trace().xyz();
    Eigen::Vector3d pr_xyz = prq_ray.trace().xyz();
    CHECK_THAT( ps_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ));
    CHECK_THAT( ps_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ));
    CHECK_THAT( ps_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ));
    
    // Duplicate of observer but with the computed lookdir result
    psmrts::PRQRayTrace prq_obs(observer, prq_ray.trace().surfpt() );
    CHECK( e_tracer.process( prq_obs ) == true ); 

    // Rigorous check of surface pointns
    Eigen::Vector3d po_xyz = prq_obs.trace().xyz();
    pr_xyz = prq_ray.trace().xyz();
    CHECK_THAT( po_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ));
    CHECK_THAT( po_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ));
    CHECK_THAT( po_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ));

    Eigen::Vector3d po_surfpt = prq_obs.trace().surfpt();
    Eigen::Vector3d pr_surfpt = prq_ray.trace().surfpt();

    CHECK_THAT( po_surfpt[0], Catch::Matchers::WithinAbs( pr_surfpt[0], tolerance ));
    CHECK_THAT( po_surfpt[1], Catch::Matchers::WithinAbs( pr_surfpt[1], tolerance ));
    CHECK_THAT( po_surfpt[2], Catch::Matchers::WithinAbs( pr_surfpt[2], tolerance ));

    // Sun Position
    Eigen::Vector3d sun_pos;
    double sun_lon = psmrts::degrees_to_radians( 20.0 ); 
    double sun_lat = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon, sun_lat, sun_pos.data() );
    sun_pos = sun_pos *  50.0; 

    // Angle between the observer and sun
    double speangle = psmrts::radians_to_degrees( psmrts::PsmrtsRayTrace::separation_angle( observer, sun_pos ) );

    // Compute the look direction from sun to surface point
    Eigen::Vector3d lookdir_s = prq_ray.trace().xyz() - sun_pos;
    psmrts::PRQRayTrace prq_sun( sun_pos, lookdir_s );
    CHECK( e_tracer.process( prq_sun ) ==  true );
    CHECK( prq_sun.trace().hasHit() == true );

    // Compute/check photometric angles
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.emission(  ) ), Catch::Matchers::WithinAbs( 5.55459887153097576, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_sun.emission(  ) ), Catch::Matchers::WithinAbs( 36.64334758469316, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.incidence( prq_sun.trace() ) ), Catch::Matchers::WithinAbs( 36.64334758469323816, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.phase( prq_sun.trace() ) ),     Catch::Matchers::WithinAbs( 32.73787834081892356, tolerance) );

    // Create Photometric trace and run
    psmrts::PRQPhotometricTrace prq_photo( observer, lookdir, sun_pos );
    CHECK( e_tracer.process( prq_photo ) == true );

    CHECK( prq_photo.isValid() == true );
    CHECK( prq_photo.observer_trace().hasHit() == true ); 
    CHECK( prq_photo.sun_trace().hasHit() == true );

    CHECK( prq_photo.observer_trace().observer() == observer );
    CHECK( prq_photo.observer_trace().lookdir() == lookdir );
    CHECK( prq_photo.observer_trace().observer() == prq_ray.trace().observer() );
    CHECK( prq_photo.observer_trace().lookdir() == prq_ray.trace().lookdir() );

    CHECK( prq_photo.sun_trace().observer() == sun_pos );
    CHECK( prq_photo.sun_trace().lookdir() == lookdir_s);
    CHECK( prq_photo.sun_trace().observer() == prq_sun.trace().observer() );
    CHECK( prq_photo.sun_trace().lookdir() == prq_sun.trace().lookdir() );

    // Compare surface intercept points of observer and sun
    Eigen::Vector3d o_xyz = prq_photo.observer_trace().xyz();
    Eigen::Vector3d s_xyz = prq_photo.sun_trace().xyz();
    CHECK_THAT( o_xyz[0], Catch::Matchers::WithinAbs( s_xyz[0], tolerance) );
    CHECK_THAT( o_xyz[1], Catch::Matchers::WithinAbs( s_xyz[1], tolerance ) );
    CHECK_THAT( o_xyz[2], Catch::Matchers::WithinAbs( s_xyz[2], tolerance ) );

    // Compute/check photometric angles compared to prq_obs above
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.emission(  ) ), Catch::Matchers::WithinAbs( 5.55459887153097576, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.incidence( ) ), Catch::Matchers::WithinAbs( 36.64334758469323816, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.phase( ) ),     Catch::Matchers::WithinAbs( 32.73787834081892356, tolerance) ); 
}
