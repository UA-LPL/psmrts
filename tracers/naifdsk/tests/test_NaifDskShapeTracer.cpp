#include <psmrts_catch2_environment.hpp>

#include <NaifDskShapeTracer.hpp>
#include <DskKernelModel.hpp>
#include <PsmrtsUtilities.hpp>

TEST_CASE("NAIF Dsk Shape Tracer - Default Constructor", "[default][naifdsk][shapetracer]"){
    const double tolerance = 1.0e-6;
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    psmrts::NaifDskShapeTracer dsk_string_tracer( dskfile );
    
    psmrts::PRQFeatures features_string;
    CHECK( dsk_string_tracer.process( features_string ) == true ); 

    CHECK( features_string.to_string() == "shapetracer" );
    CHECK( features_string.config().dump() == "shapetracer" );

    naif::DskKernelModel dsk( dskfile );
    psmrts::NaifDskShapeTracer dsk_model_tracer( dsk );

    psmrts::PRQFeatures features_model;
    CHECK( dsk_model_tracer.process (features_model) == true );

    CHECK( features_model.to_string() == features_string.to_string() );
    CHECK( features_model.config().dump() == features_string.config().dump() );
}

TEST_CASE("NAIF Dsk Shape Tracer Test", "[naifdsk][shapetracer]") {
    const double tolerance_km = 1.0e-6;

    std::string dskfile = psmrts_formats_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment segment = dsk.segment();
    psmrts::NaifDskShapeTracer d_tracer( dsk );

    Eigen::Vector3d obs;
    double radius = segment.maximum_radius();
    double obs_long = 90.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10;

    Eigen::Vector3d surf;
    double surf_lon = 90.0 * rpd_c();
    double surf_lat = 45.0 * rpd_c();
    latrec_c( radius, surf_lon, surf_lat, surf.data() );

    Eigen::Vector3d surf_obs = surf * 1.5;
    psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs);
    REQUIRE( d_tracer.process( prq_ray ) == true );

    Eigen::Vector3d lkdr = prq_ray.trace().xyz() - obs;

    psmrts::PRQRayTrace prq_spt(obs, lkdr );
    REQUIRE( d_tracer.process( prq_spt ) );

    Eigen::Vector3d normal = prq_spt.trace().normal();
    Eigen::Vector3d xyz = prq_spt.trace().xyz();

    CHECK( prq_ray.isValid() == true );
    CHECK( prq_spt.isValid() == prq_spt.trace().hasHit() );

    CHECK_THAT( normal[0], Catch::Matchers::WithinAbs( 0.0, tolerance_km));
    CHECK_THAT( normal[1], Catch::Matchers::WithinAbs( 0.0, tolerance_km));
    CHECK_THAT( normal[2], Catch::Matchers::WithinAbs( 0.0, tolerance_km));

    double d_lat, d_lon, d_radius;
    reclat_c( xyz.data(), &d_radius, &d_lon, &d_lat );

    CHECK_THAT( d_lon, Catch::Matchers::WithinAbs( 90.0 * rpd_c(), tolerance_km ));
    CHECK_THAT( d_lat, Catch::Matchers::WithinAbs( 45.0 * rpd_c(), tolerance_km ));

    CHECK_THAT( d_radius, Catch::Matchers::WithinAbs( prq_spt.trace().radius(), tolerance_km));
    CHECK_THAT( d_lon, Catch::Matchers::WithinAbs( surf_lon, tolerance_km ));
    CHECK_THAT( d_lat, Catch::Matchers::WithinAbs( surf_lat, tolerance_km ));

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[0], tolerance_km )); 
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[1], tolerance_km )); 
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[2], tolerance_km )); 
}