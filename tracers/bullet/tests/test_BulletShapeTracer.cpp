#include <psmrts_catch2_environment.hpp>

#include <PsmrtsBulletWorldModel.hpp>
#include <BulletTracerModel.hpp>
#include <BulletShapeTracer.hpp>
#include <PsmrtsOBJFormat.hpp>
#include <PsmrtsUtilities.hpp>

TEST_CASE ( "Bullet Shape Tracer - Default Constructor", "[default][bullet][shapetracer]" ) {
    psmrts::BulletShapeTracer b_tracer;

    psmrts::PRQFeatures features;
    CHECK( b_tracer.process( features ) == true );

    CHECK( features.to_string()     == "[{\"name\":\"bullet\",\"product\":\"shapetracer\",\"mesh\":true,\"optimizebvh\":\
false,\"vectortype\":[\"double\",\"float\"]}]");
    CHECK( features.config().dump() == "[{\"name\":\"bullet\",\"product\":\"shapetracer\",\"mesh\":true,\"optimizebvh\":\
false,\"vectortype\":[\"double\",\"float\"]}]");

    psmrts_json add = "tracer";
    CHECK_NOTHROW(features.add_feature(add));
    CHECK(features.to_string() == "[{\"name\":\"bullet\",\"product\":\"shapetracer\",\"mesh\":true,\"optimizebvh\":\
false,\"vectortype\":[\"double\",\"float\"]},\"tracer\"]");
    // etc...

    // Base Request Functions
    CHECK( features.name() == "PRQFeatures" ); 

    CHECK_NOTHROW( features.process_running() );
    CHECK_NOTHROW( features.process_complete() );
 
    CHECK( features.run_count()     == 1 );
    CHECK( features.was_invoked()   == true );
    CHECK( features.error_count()   == 0 );
    CHECK( features.errors().size() == 0 ); 

    CHECK_NOTHROW( features.throw_errors() ); 
    CHECK_NOTHROW( features.clear_errors() );

    
    psmrts::PRQPhotometricTrace photoTrace;
    CHECK_THROWS( b_tracer.process( photoTrace )   == false ); 
    CHECK( photoTrace.isValid()             == false );
    CHECK_THAT( photoTrace.incidence(), Catch::Matchers::IsNaN() );
    CHECK_THAT( photoTrace.emission(),  Catch::Matchers::IsNaN() );
    CHECK_THAT( photoTrace.phase(),     Catch::Matchers::IsNaN() );
    CHECK( photoTrace.compute_sun_lookdir() == false );

}

// Load a mesh and inspect contents of Bullet world

// Load mesh and run some traces

TEST_CASE( "Bullet Shape Tracer Test", "[bullet][shapetracer]" ) {
    const double tolerance_km = 1.0e-6;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );

    // This spec saves significant memory... and confirms Bullet preserves data
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
    REQUIRE( bt_world.isValid() == true );

    psmrts::BulletShapeTracer b_tracer( bt_world );

    // Compute the position of the observer at ( 45,45 ) degrees
    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    // Compute the surface point at ( 45, 50 ). This is our surface target
    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    // Find the real surface point using bullet
    Eigen::Vector3d surf_obs = surf * 1.5;
    psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs );
    REQUIRE( b_tracer.process( prq_ray ) == true );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lookdir = prq_ray.trace().xyz() - obs;

    // Trace it from observer to surface point to confirm
    psmrts::PRQRayTrace prq_spt(obs, lookdir );
    REQUIRE( b_tracer.process( prq_spt ) );
    
    Eigen::Vector3d normal = prq_spt.trace().normal();
    Eigen::Vector3d xyz = prq_spt.trace().xyz();

    // Compare expected results!
    CHECK( prq_ray.isValid() == true );
    CHECK( prq_spt.isValid() == prq_spt.trace().hasHit() );

    CHECK_THAT( normal[0], Catch::Matchers::WithinAbs(0.0,                tolerance_km ));
    CHECK_THAT( normal[1], Catch::Matchers::WithinAbs(0.5257310881115882, tolerance_km ));
    CHECK_THAT( normal[2], Catch::Matchers::WithinAbs(0.85065082318951801, tolerance_km ));

    // Compute radius/lon/lat from intercept surface point (body-fixed)
    double bt_lat, bt_lon, bt_radius;
    reclat_c( xyz.data(), &bt_radius, &bt_lon, &bt_lat);
    
    CHECK_THAT( bt_lon,    Catch::Matchers::WithinAbs( 45.0 * rpd_c(), tolerance_km ));    
    CHECK_THAT( bt_lat,    Catch::Matchers::WithinAbs( 50.0 * rpd_c(), tolerance_km ));  

    CHECK_THAT( bt_radius, Catch::Matchers::WithinAbs( prq_spt.trace().radius(), tolerance_km ));    
    CHECK_THAT( bt_lon,    Catch::Matchers::WithinAbs( surf_lon,     tolerance_km ));    
    CHECK_THAT( bt_lat,    Catch::Matchers::WithinAbs( surf_lat,     tolerance_km ));    

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[0], tolerance_km ) );
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[1], tolerance_km ) );
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[2], tolerance_km ) );

    psmrts::PRQFacet prq_facet( prq_ray.trace() );
    CHECK( prq_facet.isValid() == true );
    //CHECK( prq_facet.facet().isValid() == true ); \\shouldnt this be true..?
    CHECK( prq_facet.prq_trace().emission() == prq_ray.emission() ); 

    Eigen::Vector3d facet_xyz( prq_facet.trace().xyz() );
    CHECK_THAT( facet_xyz[0], Catch::Matchers::WithinAbs( xyz[0], tolerance_km));
    CHECK_THAT( facet_xyz[1], Catch::Matchers::WithinAbs( xyz[1], tolerance_km));
    CHECK_THAT( facet_xyz[2], Catch::Matchers::WithinAbs( xyz[2], tolerance_km));

}

#if 1
TEST_CASE( "Bullet Shape Tracer Photometric Values Test", "[bullet][shapetracer][photometric]") {
    const double tolerance = 1.0e-6;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );

    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
    psmrts::BulletShapeTracer b_tracer( bt_world );

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
    CHECK( b_tracer.process( prq_surf ) == true );
    CHECK( surf_obs == prq_surf.trace().observer() ); 

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lookdir = prq_surf.trace().xyz() - observer;
    
    // Create trace from observer to surface xyz = (45d, 50d, r km)
    psmrts::PRQRayTrace prq_ray( observer, lookdir );
    CHECK( b_tracer.process( prq_ray ) == true );
    //CHECK( prq_ray.trace().lookdir() == lookdir );

    // Rigorous check of surface points
    Eigen::Vector3d ps_xyz  = prq_surf.trace().xyz();
    Eigen::Vector3d pr_xyz  = prq_ray.trace().xyz();
    CHECK_THAT( ps_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ) );
    CHECK_THAT( ps_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ) );
    CHECK_THAT( ps_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ) );

    // Create a duplicate of observer but with the computed lookdir result
    psmrts::PRQRayTrace prq_obs(observer, prq_ray.trace().surfpt() ); 
    CHECK( b_tracer.process( prq_obs ) == true );
    
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
    CHECK( b_tracer.process( prq_sun ) == true );
    CHECK( prq_sun.trace().hasHit()    == true );
    // CHECK( prq_sun.trace().lookdir()   == lookdir_s ); - lookdir being recalculated in PsmrtsBulletWorldModel

    // Compute/check photometric angles
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.emission(  ) ), Catch::Matchers::WithinAbs( 30.3643509807580, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_sun.emission(  ) ), Catch::Matchers::WithinAbs( 62.95821025018705086, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.incidence( prq_sun.trace() ) ), Catch::Matchers::WithinAbs( 62.95821025018705086, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_obs.phase( prq_sun.trace() ) ),     Catch::Matchers::WithinAbs( 32.5950452371838324, tolerance) );

    // FINALLY create the Photometric trace and run it!
    psmrts::PRQPhotometricTrace prq_photo( observer, lookdir, sun_pos );
    CHECK( b_tracer.process( prq_photo )         == true );

    CHECK( prq_photo.isValid()                   == true );
    CHECK( prq_photo.observer_trace().hasHit()   == true );
    CHECK( prq_photo.sun_trace().hasHit()        == true );

    CHECK( prq_photo.observer_trace().observer() == observer ); 
    // CHECK( prq_photo.observer_trace().lookdir()  == lookdir  ); - lookdir being recalculated in PsmrtsBulletWorldModel
    CHECK( prq_photo.observer_trace().observer() == prq_ray.trace().observer() );
    CHECK( prq_photo.observer_trace().lookdir()  == prq_ray.trace().lookdir()  );

    CHECK( prq_photo.sun_trace().observer()      == sun_pos   );
    // CHECK( prq_photo.sun_trace().lookdir()       == lookdir_s ); - lookdir being recalculated in PsmrtsBulletWorldModel
    CHECK( prq_photo.sun_trace().observer()      == prq_sun.trace().observer() );
    CHECK( prq_photo.sun_trace().lookdir()       == prq_sun.trace().lookdir()  );

    // Compare surface intercept points of observer and sun
    Eigen::Vector3d o_xyz = prq_photo.observer_trace().xyz();
    Eigen::Vector3d s_xyz = prq_photo.sun_trace().xyz();
    CHECK_THAT( o_xyz[0], Catch::Matchers::WithinAbs( s_xyz[0], tolerance) );
    CHECK_THAT( o_xyz[1], Catch::Matchers::WithinAbs( s_xyz[1], tolerance ) );
    CHECK_THAT( o_xyz[2], Catch::Matchers::WithinAbs( s_xyz[2], tolerance ) );

   // Compute/check photometric angles compared to prt_obs above
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.emission(  ) ), Catch::Matchers::WithinAbs( 30.3643509807580, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.incidence( ) ), Catch::Matchers::WithinAbs( 62.95821025018705086, tolerance) );
    CHECK_THAT( psmrts::radians_to_degrees( prq_photo.phase( ) ),     Catch::Matchers::WithinAbs( 32.5950452371838324, tolerance) );    
}
#endif