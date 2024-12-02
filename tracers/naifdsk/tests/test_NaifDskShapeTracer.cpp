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

    CHECK( features_string.to_string() == "[[[\"name\",\"bullet\"],[\"product\",\"shapetracer\"],[\"mesh\",true]\
,[\"optimizebvh\",false],[\"vectortype\",[\"double\",\"float\"]]]]" );
    CHECK( features_string.config().dump() == "[[[\"name\",\"bullet\"],[\"product\",\"shapetracer\"],[\"mesh\",true]\
,[\"optimizebvh\",false],[\"vectortype\",[\"double\",\"float\"]]]]" );

    naif::DskKernelModel dsk( dskfile );
    psmrts::NaifDskShapeTracer dsk_model_tracer( dsk );

    psmrts::PRQFeatures features_model;
    CHECK( dsk_model_tracer.process (features_model) == true );

    CHECK( features_model.to_string() == features_string.to_string() );
    CHECK( features_model.config().dump() == features_string.config().dump() );
}

TEST_CASE("NAIF Dsk Shape Tracer Test", "[naifdsk][shapetracer]") {
    const double tolerance_km = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
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
    CHECK_THAT( normal[1], Catch::Matchers::WithinAbs( 0.5257310809272836, tolerance_km));
    CHECK_THAT( normal[2], Catch::Matchers::WithinAbs( 0.8506508276296626, tolerance_km));

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

TEST_CASE( "NAIF Dsk Shape Tracer Photometric Values Test", "[naifdsk][shapetracer][photometric]") {
    const double tolerance = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment segment= dsk.segment();
    psmrts::NaifDskShapeTracer d_tracer( dsk );

    // Compute the position of the observer at ( 45d, 45d, 10 km )
    Eigen::Vector3d observer;
    double radius = 1.0;
    double obs_long = psmrts::degrees_to_radians( 45.0 );
    double obs_lat  = psmrts::degrees_to_radians( 45.0 );
    latrec_c ( radius, obs_long, obs_lat, observer.data() );
    observer = observer * 10.0;

    // Compute the surface vector at (45, 50, 1 ). This is our surface target vector
    Eigen::Vector3d surf;
    double surf_lon = psmrts::degrees_to_radians( 45.0 );
    double surf_lat = psmrts::degrees_to_radians( 50.0 );
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    // Find the real surface point using bullet surf_obs( 45d, 50d, 1.5 km)
    Eigen::Vector3d surf_obs = surf * 1.5;
    psmrts::PRQRayTrace prq_surf(surf_obs, -surf_obs );
    CHECK( d_tracer.process( prq_surf ) == true );
    CHECK( surf_obs == prq_surf.trace().observer() ); 

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lookdir = prq_surf.trace().xyz() - observer;

    // Create trace from observer to surface xyz = (45d, 50d, r km)
    psmrts::PRQRayTrace prq_ray( observer, lookdir );
    CHECK( d_tracer.process( prq_ray ) == true );
    CHECK( prq_ray.trace().lookdir() == lookdir );

    // Rigorous check of surface points
    Eigen::Vector3d ps_xyz  = prq_surf.trace().xyz();
    Eigen::Vector3d pr_xyz  = prq_ray.trace().xyz();
    CHECK_THAT( ps_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ) );
    CHECK_THAT( ps_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ) );
    CHECK_THAT( ps_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ) );

    // Create a duplicate of observer but with the computed lookdir result
    psmrts::PRQRayTrace prq_obs(observer, prq_ray.trace().surfpt() ); 
    CHECK( d_tracer.process( prq_obs ) == true );

    // Rigorous check of surface points
    Eigen::Vector3d po_xyz  = prq_obs.trace().xyz();
    pr_xyz  = prq_ray.trace().xyz();
    CHECK_THAT( po_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ) );
    CHECK_THAT( po_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ) );
    CHECK_THAT( po_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ) );

    Eigen::Vector3d po_surfpt = prq_obs.trace().surfpt();
    Eigen::Vector3d pr_surfpt = prq_ray.trace().surfpt();

    CHECK_THAT( po_surfpt[0], Catch::Matchers::WithinAbs( pr_surfpt[0], tolerance ));
    CHECK_THAT( po_surfpt[1], Catch::Matchers::WithinAbs( pr_surfpt[1], tolerance ));
    CHECK_THAT( po_surfpt[2], Catch::Matchers::WithinAbs( pr_surfpt[2], tolerance ));

    // Set up a sun position
    Eigen::Vector3d sun_pos;
    double sun_lon = psmrts::degrees_to_radians( 20.0 );
    double sun_lat = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon, sun_lat, sun_pos.data());
    sun_pos = sun_pos * 50.0;

    // Angle between the observer and sun
    double speangle = psmrts::radians_to_degrees( psmrts::PsmrtsRayTrace::separation_angle( observer, sun_pos ) );
    CHECK_THAT( speangle, Catch::Matchers::WithinAbs( 32.4294097676788482, tolerance) );

    // Compute the look direction from sun to surface point
    Eigen::Vector3d lookdir_s = prq_ray.trace().xyz() - sun_pos;
    psmrts::PRQRayTrace prq_sun(sun_pos, lookdir_s );
    CHECK( d_tracer.process( prq_sun ) == true );
    CHECK( prq_sun.trace().hasHit()    == true );
    CHECK( prq_sun.trace().lookdir()   == lookdir_s );

    // Compute/check photometric angles
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.emission(  ) ), Catch::Matchers::WithinAbs( 30.27681520779734825, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_sun.emission(  ) ), Catch::Matchers::WithinAbs( 62.78856867179433721, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.incidence( prq_sun.trace() ) ), Catch::Matchers::WithinAbs( 62.78856867179433721, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.phase( prq_sun.trace() ) ),     Catch::Matchers::WithinAbs( 32.5121566730878726, tolerance) );

    // FINALLY create the Photometric trace and run it!
    psmrts::PRQPhotometricTrace prq_photo( observer, lookdir, sun_pos );
    CHECK( d_tracer.process( prq_photo )         == true );

    CHECK( prq_photo.isValid()                   == true );
    CHECK( prq_photo.observer_trace().hasHit()   == true );
    CHECK( prq_photo.sun_trace().hasHit()        == true );

    CHECK( prq_photo.observer_trace().observer() == observer ); 
    CHECK( prq_photo.observer_trace().lookdir()  == lookdir  );
    CHECK( prq_photo.observer_trace().observer() == prq_ray.trace().observer() );
    CHECK( prq_photo.observer_trace().lookdir()  == prq_ray.trace().lookdir()  );

    CHECK( prq_photo.sun_trace().observer()      == sun_pos   );
    CHECK( prq_photo.sun_trace().lookdir()       == lookdir_s );
    CHECK( prq_photo.sun_trace().observer()      == prq_sun.trace().observer() );
    CHECK( prq_photo.sun_trace().lookdir()       == prq_sun.trace().lookdir() );

    // Compare surface intercept points of observer and sun
    Eigen::Vector3d o_xyz = prq_photo.observer_trace().xyz();
    Eigen::Vector3d s_xyz = prq_photo.sun_trace().xyz();
    CHECK_THAT( o_xyz[0], Catch::Matchers::WithinAbs( s_xyz[0], tolerance) );
    CHECK_THAT( o_xyz[1], Catch::Matchers::WithinAbs( s_xyz[1], tolerance ) );
    CHECK_THAT( o_xyz[2], Catch::Matchers::WithinAbs( s_xyz[2], tolerance ) );

    // Compute/check photometric angles compared to prt_obs above
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.emission(  ) ), Catch::Matchers::WithinAbs( 30.27681520779735536, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.incidence( ) ), Catch::Matchers::WithinAbs( 62.78856867179433721, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.phase( ) ),     Catch::Matchers::WithinAbs( 32.5121566730878726, tolerance) );   

    // Should values be this different from Bullet version?
    // Related to lookdir calculation handling?

}
