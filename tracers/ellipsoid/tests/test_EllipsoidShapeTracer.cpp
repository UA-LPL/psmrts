#include <psmrts_catch2_environment.hpp>

#include <EllipsoidShapeTracer.hpp>
#include <EllipsoidTracerModel.hpp>
#include <PsmrtsUtilities.hpp>

TEST_CASE( "Ellipsoid Shape Tracer - Request Default Constructor", "[default][ellipsoid][shapetracer]") {
    psmrts::EllipsoidShapeTracer e_tracer;

    psmrts::PRQFeatures features;
    CHECK( e_tracer.process( features ) == true );

    CHECK( features.to_string() == "[[\"radii\",[1,2,3]]]" ); //  inconsistent compared to bullet version
    CHECK( features.config().dump() == "[[\"radii\",[1,2,3]]]"); // same as above (where is name? product?)

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