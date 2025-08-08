#include <psmrts/core/testspsmrts_catch2_environment.hpp>

#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>
#include <DskKernelModel.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>

TEST_CASE("NAIF Dsk Shape Tracer - Default Constructor", "[default][naifdsk][shapetracer]"){
    const double tolerance = 1.0e-6;
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    psmrts::NaifDskTracer dsk_string_tracer( dskfile );
    
    psmrts::PRQFeatures features_string;
    CHECK( dsk_string_tracer.process( features_string ) == true ); 

    nlohmann::ordered_json j_output;
    nlohmann::ordered_json j_add;
    j_add += { "name" , "naifdsk" };
    j_add += { "product" , "shapetracer" };
    j_add += { "mesh" , true };
    j_output += j_add;

    auto feat_diff = nlohmann::ordered_json::diff(features_string.config(), j_output);
    CHECK( feat_diff.empty() );
    
    CHECK( features_string.to_string() == features_string.config().dump());

    naif::DskKernelModel dsk( dskfile );
    psmrts::NaifDskTracer dsk_model_tracer( dsk );

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
    psmrts::NaifDskTracer d_tracer( dsk );

    const double max_radius = dsk.maximum_radius();

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

    Eigen::Vector3d surf_obs = surf * (max_radius + 1.5);
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

    // Compare Values to OBJ/Bullet - should they be the same given similar parameters?
    psmrts::PRQFacet prq_facet( prq_ray.trace() );
    CHECK( prq_facet.isValid() == true );
    CHECK( d_tracer.process( prq_facet ) );
    CHECK( prq_facet.facet().isValid() == true ); 
    CHECK( prq_facet.prq_trace().emission() == prq_ray.emission() ); 

    CHECK( prq_facet.trace().segment_number() == 2101955 );
    CHECK( prq_facet.trace().plateid()        == 31 ); 
    CHECK( prq_facet.facet().m_indexes[0]     == 12 );
    CHECK( prq_facet.facet().m_indexes[1]     == 11 );
    CHECK( prq_facet.facet().m_indexes[2]     == 5 );

    CHECK_THAT( prq_facet.facet().m_normal[0], Catch::Matchers::WithinAbs( 0.00000002599305449, tolerance_km));
    CHECK_THAT( prq_facet.facet().m_normal[1], Catch::Matchers::WithinAbs( 0.52573108811158831, tolerance_km));
    CHECK_THAT( prq_facet.facet().m_normal[2], Catch::Matchers::WithinAbs( 0.85065082318951801, tolerance_km));
    
    CHECK_THAT( prq_facet.facet().m_vector1[0], Catch::Matchers::WithinAbs( -0.10100385653540001, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector1[1], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector1[2], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );

    CHECK_THAT( prq_facet.facet().m_vector2[0], Catch::Matchers::WithinAbs( 0.10100385653540001, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector2[1], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector2[2], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );

    CHECK_THAT( prq_facet.facet().m_vector3[0], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector3[1], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector3[2], Catch::Matchers::WithinAbs( 0.10100385653540001, tolerance_km ) );
}

TEST_CASE( "NAIF Dsk Shape Tracer Ray Trace Array Test", "[naifdsk][shapetracer][raytrace][array]") {
    const double tolerance = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment segment = dsk.segment();
    psmrts::NaifDskTracer d_tracer( dsk );

    const double max_radius = dsk.maximum_radius();

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

    Eigen::Vector3d surf_obs1 = surf1 * (max_radius + 1.5);
    psmrts::PRQRayTrace prq_ray1(surf_obs1, -surf_obs1 );
    REQUIRE( d_tracer.process( prq_ray1 ) == true );

    Eigen::Vector3d lookdir1 = prq_ray1.trace().xyz() - obs1;

    psmrts::PRQRayTrace prq_spt1( obs1, lookdir1 );
    REQUIRE( d_tracer.process( prq_spt1 ) );
    CHECK( prq_spt1.trace().hasHit() == true );

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

    Eigen::Vector3d surf_obs2 = surf2 * (max_radius + 1.5);
    psmrts::PRQRayTrace prq_ray2(surf_obs2, -surf_obs2 );
    REQUIRE( d_tracer.process( prq_ray2 ) == true );

    Eigen::Vector3d lookdir2 = prq_ray2.trace().xyz() - obs2;

    psmrts::PRQRayTrace prq_spt2(obs2, lookdir2 );
    REQUIRE( d_tracer.process( prq_spt2 ) );
    CHECK( prq_spt2.trace().hasHit() == true ); 

    // Ray Trace 3 (No hit condition)
    Eigen::Vector3d obs3;
    double radius3 = 1.0;
    double obs_long3 = 45.0 * rpd_c();
    double obs_lat3 = 45.0 * rpd_c();
    latrec_c ( radius3, obs_long3, obs_lat3, obs3.data() );
    obs3 = obs3 * 10.0;

    Eigen::Vector3d surf3;
    double surf_lon3 = 120.0 * rpd_c();
    double surf_lat3 = -45.0 * rpd_c();
    latrec_c ( radius3, surf_lon3, surf_lat3, surf3.data() );

    Eigen::Vector3d lookdir3 = surf3 - obs3; 

    psmrts::PRQRayTrace prq_spt3(obs3, lookdir3 );
    CHECK( d_tracer.process( prq_spt3 ) == false );
    CHECK( prq_spt3.trace().hasHit() == false ); 

    psmrts::PRQRayTraceArray ray_array;
    // empty, no hits
    CHECK( d_tracer.process( ray_array ) == false );

    // add one miss - should still be false
    ray_array.add_trace(prq_spt3);
    CHECK( d_tracer.process( ray_array ) == false );

    // add two hits
    ray_array.add_trace(prq_spt1);
    ray_array.add_trace(prq_spt2);

    // needs at least one hit to be true
    CHECK ( d_tracer.process( ray_array ) == true );
}

TEST_CASE( "NAIF Dsk Shape Tracer Photometric Values Test", "[naifdsk][shapetracer][photometric]") {
    const double tolerance = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment segment= dsk.segment();
    psmrts::NaifDskTracer d_tracer( dsk );

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
    psmrts::PRQRayTrace prq_obs(observer, prq_ray.trace().raypt() ); 
    CHECK( d_tracer.process( prq_obs ) == true );

    // Rigorous check of surface points
    Eigen::Vector3d po_xyz  = prq_obs.trace().xyz();
    pr_xyz  = prq_ray.trace().xyz();
    CHECK_THAT( po_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ) );
    CHECK_THAT( po_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ) );
    CHECK_THAT( po_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ) );

    Eigen::Vector3d po_raypt = prq_obs.trace().raypt();
    Eigen::Vector3d pr_raypt = prq_ray.trace().raypt();

    CHECK_THAT( po_raypt[0], Catch::Matchers::WithinAbs( pr_raypt[0], tolerance ));
    CHECK_THAT( po_raypt[1], Catch::Matchers::WithinAbs( pr_raypt[1], tolerance ));
    CHECK_THAT( po_raypt[2], Catch::Matchers::WithinAbs( pr_raypt[2], tolerance ));

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

TEST_CASE( "NAIF Dsk Shape Tracer Photometric Array Test", "[naifdsk][shapetracer][photometric][array]") {
    const double tolerance = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment segment = dsk.segment();
    psmrts::NaifDskTracer d_tracer( dsk );

    const double max_radius = dsk.maximum_radius();

    // Photometric Trace 1
    Eigen::Vector3d observer1;
    double radius = 1.0;
    double obs_long1 = psmrts::degrees_to_radians( 45.0 );
    double obs_lat1  = psmrts::degrees_to_radians( 45.0 );
    latrec_c ( radius, obs_long1, obs_lat1, observer1.data() );
    observer1 = observer1 * 10.0;

    Eigen::Vector3d surf1;
    double surf_lon1 = psmrts::degrees_to_radians( 45.0 );
    double surf_lat1 = psmrts::degrees_to_radians( 50.0 );
    latrec_c ( radius, surf_lon1, surf_lat1, surf1.data() );

    Eigen::Vector3d surf_obs1 = surf1 * (max_radius + 1.5);
    psmrts::PRQRayTrace prq_surf1(surf_obs1, -surf_obs1 );
    CHECK( d_tracer.process( prq_surf1 ) == true );
    CHECK( surf_obs1 == prq_surf1.trace().observer() );

    Eigen::Vector3d lookdir1 = prq_surf1.trace().xyz() - observer1;

    psmrts::PRQRayTrace prq_ray1( observer1, lookdir1 );
    CHECK( d_tracer.process( prq_ray1 ) == true );

    Eigen::Vector3d sun_pos1;
    double sun_lon1 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat1 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon1, sun_lat1, sun_pos1.data());
    sun_pos1 = sun_pos1 * 50.0;

    Eigen::Vector3d lookdir_s1 = prq_ray1.trace().xyz() - sun_pos1;
    psmrts::PRQRayTrace prq_sun1(sun_pos1, lookdir_s1 );
    CHECK( d_tracer.process( prq_sun1 ) == true );
    CHECK( prq_sun1.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo1( observer1, lookdir1, sun_pos1 );
    CHECK( d_tracer.process( prq_photo1 )         == true );
    CHECK( prq_photo1.isValid()                   == true );
    CHECK( prq_photo1.observer_trace().hasHit()   == true );
    CHECK( prq_photo1.sun_trace().hasHit()        == true );

    // Photometric Trace 2
    Eigen::Vector3d observer2;
    double obs_long2 = psmrts::degrees_to_radians( 45.0 );
    double obs_lat2  = psmrts::degrees_to_radians( 45.0 );
    latrec_c ( radius, obs_long2, obs_lat2, observer2.data() );
    observer2 = observer2 * 10.0;

    Eigen::Vector3d surf2;
    double surf_lon2 = psmrts::degrees_to_radians( 50.0 );
    double surf_lat2 = psmrts::degrees_to_radians( 45.0 );
    latrec_c ( radius, surf_lon2, surf_lat2, surf2.data() );

    Eigen::Vector3d surf_obs2 = surf2 * (max_radius + 1.5);
    psmrts::PRQRayTrace prq_surf2(surf_obs2, -surf_obs2 );
    CHECK( d_tracer.process( prq_surf2 ) == true );
    CHECK( surf_obs2 == prq_surf2.trace().observer() );

    Eigen::Vector3d lookdir2 = prq_surf2.trace().xyz() - observer2;

    psmrts::PRQRayTrace prq_ray2( observer2, lookdir2 );
    CHECK( d_tracer.process( prq_ray2 ) == true );

    Eigen::Vector3d sun_pos2;
    double sun_lon2 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat2 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon2, sun_lat2, sun_pos2.data());
    sun_pos2 = sun_pos2 * 50.0;

    Eigen::Vector3d lookdir_s2 = prq_ray2.trace().xyz() - sun_pos2;
    psmrts::PRQRayTrace prq_sun2(sun_pos2, lookdir_s2 );
    CHECK( d_tracer.process( prq_sun2 ) == true );
    CHECK( prq_sun2.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo2( observer2, lookdir2, sun_pos2 );
    CHECK( d_tracer.process( prq_photo2 )         == true );
    CHECK( prq_photo2.isValid()                   == true );
    CHECK( prq_photo2.observer_trace().hasHit()   == true );
    CHECK( prq_photo2.sun_trace().hasHit()        == true );

    // Photometric Trace 3 (No hit condition for observer / sun)
    Eigen::Vector3d observer3;
    double obs_long3 = psmrts::degrees_to_radians( 45.0 );
    double obs_lat3  = psmrts::degrees_to_radians( 45.0 );
    latrec_c ( radius, obs_long3, obs_lat3, observer3.data() );
    observer3 = observer3 * 10.0;

    Eigen::Vector3d surf3;
    double surf_lon3 = psmrts::degrees_to_radians( 120.0 );
    double surf_lat3 = psmrts::degrees_to_radians( -45.0 );
    latrec_c ( radius, surf_lon3, surf_lat3, surf3.data() );

    Eigen::Vector3d lookdir3 = surf3 - observer3;

    psmrts::PRQRayTrace prq_ray3( observer3, lookdir3 );
    CHECK( d_tracer.process( prq_ray3 ) == false );
    CHECK( prq_ray3.trace().hasHit() == false );

    Eigen::Vector3d sun_pos3;
    double sun_lon3 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat3 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon3, sun_lat3, sun_pos3.data());
    sun_pos3 = sun_pos3 * 50.0;

    Eigen::Vector3d lookdir_s3 = prq_ray3.trace().xyz() - sun_pos3;
    psmrts::PRQRayTrace prq_sun3(sun_pos3, lookdir_s3 );
    CHECK( d_tracer.process( prq_sun3 ) == true ); // Should be false?
    CHECK( prq_sun3.trace().hasHit()    == true ); // should be false?

    psmrts::PRQPhotometricTrace prq_photo3( observer3, lookdir3, sun_pos3 );
    CHECK( d_tracer.process( prq_photo3 )         == false );
    CHECK( prq_photo3.isValid()                   == false );
    CHECK( prq_photo3.observer_trace().hasHit()   == false );
    CHECK( prq_photo3.sun_trace().hasHit()        == false );

    psmrts::PRQPhotometricTraceArray photo_array;
    CHECK( d_tracer.process( photo_array ) == false );

    // add one miss, should still be false
    photo_array.add_trace( prq_photo3 );
    CHECK( d_tracer.process( photo_array ) == false );

    // add two hits, should now be true - array needs at least 1 hit
    photo_array.add_trace( prq_photo1 );
    photo_array.add_trace( prq_photo2 );
    CHECK( d_tracer.process( photo_array ) == true );
}

// This test compares facet data resulting from a dsk Ray Trace to a PRQRequest Ray Trace,
// ensuring they target the same segment/plate.
TEST_CASE( "NAIF Dsk Shape Tracer Ray-Facet Test", "[naifdsk][shapetracer][raytrace][facet]") {
    const double tolerance_km = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    psmrts::NaifDskTracer d_tracer( dsk );

    const double max_radius = dsk.maximum_radius();

    double dsk_lon = GENERATE( -360.0, -245.0, -175.0, -90.0, -45.0, 0.0, 45.0, 90.0, 175.0, 245.0, 360.0 );
    double dsk_lat = GENERATE( -90.0, -75.0, -45.0, 0.0, 45.0, 75.0, 90.0 );

    Eigen::Vector3d surf;
    latrec_c( 1.0, dsk_lon, dsk_lat, surf.data() ); 

    Eigen::Vector3d surf_obs = surf * (max_radius + 1.5 );
    psmrts::PRQRayTrace prq_ray( surf_obs, -surf_obs );

    CHECK( d_tracer.process( prq_ray ));
    psmrts::PRQFacet prq_facet( prq_ray.trace() );
    CHECK( prq_facet.isValid() == true );
    CHECK( d_tracer.process( prq_facet ) );

    psmrts::PsmrtsRayTrace dsk_ray;
    CHECK( dsk.ray_trace( surf_obs, -surf_obs, dsk_ray) );
    psmrts::PsmrtsRayTrace::FacetDatum dsk_facet;
    CHECK( dsk.get_facet( dsk_ray, dsk_facet ) );

    CHECK( prq_facet.trace().segment_number() == dsk_ray.datum().m_segment );
    CHECK( prq_facet.trace().plateid()        == dsk_ray.datum().m_plateid );
    CHECK( prq_facet.facet().m_indexes        == dsk_facet.m_indexes );

    CHECK_THAT( prq_facet.facet().m_normal[0], Catch::Matchers::WithinAbs( dsk_facet.m_normal[0], tolerance_km));
    CHECK_THAT( prq_facet.facet().m_normal[1], Catch::Matchers::WithinAbs( dsk_facet.m_normal[1], tolerance_km));
    CHECK_THAT( prq_facet.facet().m_normal[2], Catch::Matchers::WithinAbs( dsk_facet.m_normal[2], tolerance_km));
    
    CHECK_THAT( prq_facet.facet().m_vector1[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[0], tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector1[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[1], tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector1[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[2], tolerance_km ) );

    CHECK_THAT( prq_facet.facet().m_vector2[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[0], tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector2[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[1], tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector2[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[2], tolerance_km ) );

    CHECK_THAT( prq_facet.facet().m_vector3[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[0], tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector3[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[1], tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector3[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[2], tolerance_km ) );
}

TEST_CASE( "NAIF Dsk Shape Tracer Product Specification Test", "[naifdsk][shapetracer][product][specification]") {
    psmrts::ProductSpecification spec = psmrts::NaifDskTracer::product_specifications();

    CHECK( spec.name()              == "naifdsk"     );
    CHECK( spec.product()           == "shapetracer" ); 
    CHECK( spec.type()              == "tracer"      );
    CHECK( spec.driver().name()     == "naifdsk"     ); 
    CHECK( spec.size()              == 2             );
    CHECK( spec.parameters().size() == 2             );
    CHECK( spec.required().size()   == 0             );
    CHECK( spec.optional().size()   == 2             );

    CHECK( spec.has_parameter( "obj_mtl_search_path" )    == false );
    CHECK( spec.has_parameter( "naif_dsk_kernel_paths" )  == true  );

}
