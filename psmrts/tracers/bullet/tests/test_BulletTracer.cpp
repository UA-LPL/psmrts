#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/bullet/BulletTracer.hpp>

#include <cspice/SpiceUsr.h>

TEST_CASE ( "Bullet Tracer - Default Constructor", "[default][bullet][tracer]" ) {

    CHECK( sizeof( psmrts::BulletTracer ) <= 470 );  

    psmrts::BulletTracer b_tracer;

    psmrts::PRQFeatures features;
    CHECK( b_tracer.process( features ) == true );

    nlohmann::ordered_json j_output;
    nlohmann::ordered_json j_add;
    j_add["name"]        = "bullet";
    j_add["product"]     = "tracer";
    j_add["mesh"]        = true;
    j_add["optimizebvh"] = false;
    j_add["vectortype"]  = { "double", "float" };
    j_output += j_add;

    auto feat_diff = nlohmann::ordered_json::diff(features.config(), j_output);

    CHECK( feat_diff.empty() );

    CHECK( features.to_string() == features.config().dump() );
    
    j_output += "tracer";
    psmrts_json add = "tracer";
    CHECK_NOTHROW(features.add_feature(add));
    
    auto feat_add = nlohmann::ordered_json::diff(features.config(), j_output );

    CHECK( feat_add.empty() );

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
    CHECK( b_tracer.process( photoTrace )          == false ); 
    CHECK( photoTrace.isValid()                    == false );
    CHECK( photoTrace.observer_trace().hasHit()    == false );
    CHECK( photoTrace.sun_trace().hasHit()         == false );

    CHECK_THAT( photoTrace.incidence(), Catch::Matchers::IsNaN() );
    CHECK_THAT( photoTrace.emission(),  Catch::Matchers::IsNaN() );
    CHECK_THAT( photoTrace.phase(),     Catch::Matchers::IsNaN() );
    CHECK( photoTrace.compute_sun_lookdir()        == false );

    // Photometric Array Default
    psmrts::PRQPhotometricTraceArray pt_array;
    CHECK( pt_array.size()           == 0 );
    CHECK( pt_array.traces().empty() == true );

    pt_array.add_trace( photoTrace );

    CHECK( pt_array.size() == 1 );

    psmrts::PRQPhotometricTrace photoTrace2;
    pt_array.add_trace( photoTrace2 );

    CHECK( pt_array.size() == 2 );

    CHECK( b_tracer.process( pt_array ) == false ); 
    
    // Photometric Array List Default
    std::vector<psmrts::PRQPhotometricTrace> pt_list;
    pt_list.push_back( photoTrace );
    pt_list.push_back( photoTrace2 );

    psmrts::PRQPhotometricTraceArray pt_array2 ( pt_list );
    CHECK( pt_array2.size() == 2 );
    CHECK( pt_array2.traces().empty() == false );

    CHECK( b_tracer.process( pt_array2 ) == false ); 

    // Ray Trace Array Default
    psmrts::PRQRayTraceArray rt_array;
    CHECK( rt_array.size() == 0 );
    CHECK( rt_array.traces().empty() == true );

    psmrts::PRQRayTrace ray;
    rt_array.add_trace( ray );

    CHECK( rt_array.size() == 1 );

    psmrts::PRQRayTrace ray2;
    rt_array.add_trace( ray2 );

    CHECK( rt_array.size() == 2 );

    CHECK( b_tracer.process( rt_array )  == false ); 

    // Ray Trace Array List Default
    std::vector<psmrts::PRQRayTrace> rt_list;
    rt_list.push_back( ray );
    rt_list.push_back( ray2 );

    psmrts::PRQRayTraceArray rt_array2( rt_list );
    CHECK( rt_array2.size() == 2 );
    CHECK( rt_array2.traces().empty() == false  );

    CHECK( b_tracer.process( rt_array2 ) == false ); 
}

TEST_CASE( "Bullet Tracer Test - Ray Trace / Values", "[bullet][tracer][values]" ) {
    const double tolerance_km = 1.0e-6;

    const std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );

    // Beware the most vexing parse (see https://www.fluentcpp.com/2018/01/30/most-vexing-parse/)
    psmrts::BulletTracer b_tracer( psmrts::PsmrtsShape{ objfile } );
    const double max_radius = b_tracer.maximum_radius();
    
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
    Eigen::Vector3d surf_obs = surf * (max_radius + 1.5);
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
    CHECK( b_tracer.process( prq_facet ) );
    CHECK( prq_facet.facet().isValid() == true ); 
    CHECK( prq_facet.prq_trace().emission() == prq_ray.emission() ); 

    Eigen::Vector3d facet_xyz( prq_facet.trace().xyz() );
    CHECK_THAT( facet_xyz[0], Catch::Matchers::WithinAbs( xyz[0], tolerance_km));
    CHECK_THAT( facet_xyz[1], Catch::Matchers::WithinAbs( xyz[1], tolerance_km));
    CHECK_THAT( facet_xyz[2], Catch::Matchers::WithinAbs( xyz[2], tolerance_km));

    // verify facet and trace segment and plate ids are identical
    CHECK( prq_facet.facet().m_segment == prq_ray.trace().segment_number() );
    CHECK( prq_facet.facet().m_plateid == prq_ray.trace().plateid() );

    // get plate id, segment id (may always be 0 in bullet)
    CHECK( prq_facet.trace().segment_number() == 0 );
    CHECK( prq_facet.trace().plateid()        == 30 ); 
    CHECK( prq_facet.facet().m_indexes[0]     == 11 );
    CHECK( prq_facet.facet().m_indexes[1]     == 14 );
    CHECK( prq_facet.facet().m_indexes[2]     == 5 );

    CHECK_THAT( prq_facet.facet().m_normal[0], Catch::Matchers::WithinAbs( 0.00000002599305449, tolerance_km));
    CHECK_THAT( prq_facet.facet().m_normal[1], Catch::Matchers::WithinAbs( 0.52573108811158831, tolerance_km));
    CHECK_THAT( prq_facet.facet().m_normal[2], Catch::Matchers::WithinAbs( 0.85065082318951801, tolerance_km));
    
    CHECK_THAT( prq_facet.facet().m_vector1[0], Catch::Matchers::WithinAbs( 0.10100385653540001, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector1[1], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector1[2], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );

    CHECK_THAT( prq_facet.facet().m_vector2[0], Catch::Matchers::WithinAbs( 0.1634276539482 , tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector2[1], Catch::Matchers::WithinAbs( 0.1634276539482, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector2[2], Catch::Matchers::WithinAbs( 0.1634276539482, tolerance_km ) );

    CHECK_THAT( prq_facet.facet().m_vector3[0], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector3[1], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );
    CHECK_THAT( prq_facet.facet().m_vector3[2], Catch::Matchers::WithinAbs( 0.10100385653540001, tolerance_km ) );

}

TEST_CASE( "Bullet Tracer Ray Trace Array Test", "[bullet][tracer][raytrace][array]") {
    const double tolerance = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" ); 
    psmrts::BulletTracer b_tracer( psmrts::PsmrtsShape{ objfile } );

    // Max radius of object: 0.28306500000006685
    const double max_radius = b_tracer.maximum_radius();

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
    REQUIRE( b_tracer.process( prq_ray1 ) == true );

    Eigen::Vector3d lookdir1 = prq_ray1.trace().xyz() - obs1;

    psmrts::PRQRayTrace prq_spt1( obs1, lookdir1 );
    REQUIRE( b_tracer.process( prq_spt1 ) );
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
    REQUIRE( b_tracer.process( prq_ray2 ) == true );

    Eigen::Vector3d lookdir2 = prq_ray2.trace().xyz() - obs2;

    psmrts::PRQRayTrace prq_spt2(obs2, lookdir2 );
    REQUIRE( b_tracer.process( prq_spt2 ) );
    CHECK( prq_spt2.trace().hasHit() == true ); 

    // Ray Trace 3 (No hit condition)
    Eigen::Vector3d obs3;
    double radius3 = 1.0;
    double obs_long3 = 45.0 * rpd_c();
    double obs_lat3 = 45.0 * rpd_c();
    latrec_c ( radius3, obs_long3, obs_lat3, obs3.data() );
    obs3 = obs3 * 10.0;

    Eigen::Vector3d surf3;
    double surf_lon3 = 135.0 * rpd_c();
    double surf_lat3 = 45.0 * rpd_c();
    latrec_c ( radius3, surf_lon3, surf_lat3, surf3.data() );

    Eigen::Vector3d lookdir3 = surf3 - obs3; 

    // Check to see where the fail intercepts on the surface
    // These tests should be true, but are not
    psmrts::PRQRayTrace prq_spt3(obs3, lookdir3 );
    CHECK( b_tracer.process( prq_spt3 ) == false );
    CHECK( prq_spt3.trace().hasHit() == false ); 

    psmrts::PRQRayTraceArray ray_array;
    // empty, no hits
    CHECK( b_tracer.process( ray_array ) == false );

    // add one miss - should still be false
    ray_array.add_trace(prq_spt3);
    CHECK( b_tracer.process( ray_array ) == false );

    // add two hits
    ray_array.add_trace(prq_spt1);
    ray_array.add_trace(prq_spt2);

    // needs at least one hit to be true
    CHECK ( b_tracer.process( ray_array ) == true );

}

TEST_CASE( "Bullet Tracer Photometric Values Test", "[bullet][tracer][photometric]") {
    const double tolerance = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::BulletTracer b_tracer( psmrts::PsmrtsShape{ objfile } );

    // Max radius of object: 0.28306500000006685
    const double max_radius = b_tracer.maximum_radius();

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
    Eigen::Vector3d surf_obs = surf * (max_radius + 1.5);
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
    psmrts::PRQRayTrace prq_obs(observer, prq_ray.trace().raypt() ); 
    CHECK( b_tracer.process( prq_obs ) == true );
    
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

TEST_CASE( "Bullet Tracer Photometric Array Test", "[bullet][tracer][photometric][array]") {
    const double tolerance = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::BulletTracer b_tracer( psmrts::PsmrtsShape{ objfile } );

    // Max radius of object: 0.28306500000006685
    const double max_radius = b_tracer.maximum_radius();

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
    CHECK( b_tracer.process( prq_surf1 ) == true );
    CHECK( surf_obs1 == prq_surf1.trace().observer() );

    Eigen::Vector3d lookdir1 = prq_surf1.trace().xyz() - observer1;

    psmrts::PRQRayTrace prq_ray1( observer1, lookdir1 );
    CHECK( b_tracer.process( prq_ray1 ) == true );

    Eigen::Vector3d sun_pos1;
    double sun_lon1 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat1 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon1, sun_lat1, sun_pos1.data());
    sun_pos1 = sun_pos1 * 50.0;

    Eigen::Vector3d lookdir_s1 = prq_ray1.trace().xyz() - sun_pos1;
    psmrts::PRQRayTrace prq_sun1(sun_pos1, lookdir_s1 );
    CHECK( b_tracer.process( prq_sun1 ) == true );
    CHECK( prq_sun1.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo1( observer1, lookdir1, sun_pos1 );
    CHECK( b_tracer.process( prq_photo1 )         == true );
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
    CHECK( b_tracer.process( prq_surf2 ) == true );
    CHECK( surf_obs2 == prq_surf2.trace().observer() );

    Eigen::Vector3d lookdir2 = prq_surf2.trace().xyz() - observer2;

    psmrts::PRQRayTrace prq_ray2( observer2, lookdir2 );
    CHECK( b_tracer.process( prq_ray2 ) == true );

    Eigen::Vector3d sun_pos2;
    double sun_lon2 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat2 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon2, sun_lat2, sun_pos2.data());
    sun_pos2 = sun_pos2 * 50.0;

    Eigen::Vector3d lookdir_s2 = prq_ray2.trace().xyz() - sun_pos2;
    psmrts::PRQRayTrace prq_sun2(sun_pos2, lookdir_s2 );
    CHECK( b_tracer.process( prq_sun2 ) == true );
    CHECK( prq_sun2.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo2( observer2, lookdir2, sun_pos2 );
    CHECK( b_tracer.process( prq_photo2 )         == true );
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
    CHECK( b_tracer.process( prq_ray3 ) == false );
    CHECK( prq_ray3.trace().hasHit() == false );

    Eigen::Vector3d sun_pos3;
    double sun_lon3 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat3 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon3, sun_lat3, sun_pos3.data());
    sun_pos3 = sun_pos3 * 50.0;

    Eigen::Vector3d lookdir_s3 = prq_ray3.trace().xyz() - sun_pos3;
    psmrts::PRQRayTrace prq_sun3(sun_pos3, lookdir_s3 );
    CHECK( b_tracer.process( prq_sun3 ) == false);
    CHECK( prq_sun3.trace().hasHit()    == false );

    psmrts::PRQPhotometricTrace prq_photo3( observer3, lookdir3, sun_pos3 );
    CHECK( b_tracer.process( prq_photo3 )         == false );
    CHECK( prq_photo3.isValid()                   == false );
    CHECK( prq_photo3.observer_trace().hasHit()   == false );
    CHECK( prq_photo3.sun_trace().hasHit()        == false );

    psmrts::PRQPhotometricTraceArray photo_array;
    CHECK( b_tracer.process( photo_array ) == false );

    // add one miss, should still be false
    photo_array.add_trace( prq_photo3 );
    CHECK( b_tracer.process( photo_array ) == false );

    // add two hits, should now be true - array needs at least 1 hit
    photo_array.add_trace( prq_photo1 );
    photo_array.add_trace( prq_photo2 );
    CHECK( b_tracer.process( photo_array ) == true );

}

TEST_CASE( "Bullet Tracer Product Specification Test", "[bullet][tracer][product][specification]") {
    psmrts::ProductSpecification spec = psmrts::BulletTracer::product_specifications();

    CHECK( spec.name()              == "bullet"      );
    CHECK( spec.product()           == "tracer" ); 
    CHECK( spec.size()              == 4 );
    CHECK( spec.features().size()   == 4 );
    CHECK( spec.required().size()   == 0 );
    CHECK( spec.optional().size()   == 3 );
    CHECK( spec.dependency().size()  == 1 );

    CHECK( spec.contains( "obj_mtl_search_path" )  == false );
    CHECK( spec.contains( "bullet_optimize_bvh" )  == true  );

}
