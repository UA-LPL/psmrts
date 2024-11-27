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

#if 0
TEST_CASE( "Ellipsoid Shape Tracer Photometric Values Test", "[ellipsoid][shapetracer][photometric]") {
    const double tolerance = 1.0e-6;

    Eigen::Vector3d radii( {0.5, 0.5, 0.5} );
    psmrts::EllipsoidShapeTracer e_tracer( radii );

    #if 0
    Eigen::Vector3d obs = {1.0, 1.0, 1.0};
    Eigen::Vector3d surf = {0.0, 0.5, 0.0};
    Eigen::Vector3d lkdr = -obs + surf;

    Eigen::Vector3d surf2 = {0.0, 0.5, 0.1};
    Eigen::Vector3d sun_pos = -obs + (surf2 * 10.0);

    psmrts::PRQPhotometricTrace e_photo( obs, lkdr, sun_pos );
    REQUIRE( e_photo.isValid() == true );
    #endif

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

    Eigen::Vector3d sun_obs;
    double sun_long = 45.5 * rpd_c();
    double sun_lat = 45.0 * rpd_c();
    latrec_c( radius*20.0, sun_long, sun_lat, sun_obs.data());
    

    psmrts::PRQPhotometricTrace e_photo ( obs, lkdr, sun_obs );
    REQUIRE( e_photo.isValid() == true );

}
#endif