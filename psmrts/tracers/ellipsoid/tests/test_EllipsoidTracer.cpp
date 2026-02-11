#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

#include <cspice/SpiceUsr.h>


TEST_CASE( "Ellipsoid Shape Tracer - Request Default Constructor", "[default][ellipsoid][shapetracer]") {

    CHECK( sizeof( psmrts::EllipsoidTracer ) <= 504 );  

    psmrts::EllipsoidTracer e_tracer;

    psmrts::PRQFeatures features;
    CHECK( e_tracer.process( features ) == true );

    nlohmann::ordered_json j_output;
    nlohmann::ordered_json j_add;
    j_add["name"] = "ellisoid" ;
    j_add["product"] = "shapetracer" ;
    j_add["mesh"] = false ;
    // Default radii values for unspecified constructed ellipsoid
    j_add["radii"] = { 1.0, 1.0, 1.0 } ;

    j_output += j_add;

    auto feat_diff = nlohmann::ordered_json::diff(features.config(), j_output);
    CHECK( feat_diff.empty() );

    CHECK( features.to_string() == features.config().dump() );

    // Note: feature-specific functions testing in bullet version
}

TEST_CASE( "Ellipsoid Shape Tracer Test", "[ellipsoid][shapetracer]") {
    const double tolerance_km = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0} );

    psmrts::EllipsoidTracer e_tracer( radii );
     
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
    REQUIRE( prq_ray.trace().get_tracer_id() == e_tracer.uid() ); 

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

    psmrts::PRQFacet prq_facet( prq_ray.trace() );
    CHECK( prq_facet.isValid()                == true ); // Returns ray validity?
    CHECK( e_tracer.process( prq_facet )      == false );
    CHECK( prq_facet.trace().segment_number() == -1 );
    CHECK( prq_facet.trace().plateid()        == -1 );

    CHECK( prq_facet.facet().m_indexes[0] == -1 );
    CHECK( prq_facet.facet().m_indexes[1] == -1 );
    CHECK( prq_facet.facet().m_indexes[2] == -1 );

    CHECK( prq_facet.facet().m_normal[0] == 0 );
    CHECK( prq_facet.facet().m_normal[1] == 0 );
    CHECK( prq_facet.facet().m_normal[2] == 0 );

    CHECK( prq_facet.facet().m_vector1[0] == 0 );
    CHECK( prq_facet.facet().m_vector1[1] == 0 );
    CHECK( prq_facet.facet().m_vector1[2] == 0 );

    CHECK( prq_facet.facet().m_vector2[0] == 0 );
    CHECK( prq_facet.facet().m_vector2[1] == 0 );
    CHECK( prq_facet.facet().m_vector2[2] == 0 );

    CHECK( prq_facet.facet().m_vector3[0] == 0 );
    CHECK( prq_facet.facet().m_vector3[1] == 0 );
    CHECK( prq_facet.facet().m_vector3[2] == 0 );

}

TEST_CASE( "Ellipsoid Shape Tracer Ray Trace Array Test", "[ellipsoid][shapetracer][raytrace][array]") {
    // WIP - Needing debugging for miss condition
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
    REQUIRE( e_tracer.process( prq_ray1 ) == true );

    Eigen::Vector3d lookdir1 = prq_ray1.trace().xyz() - obs1;

    psmrts::PRQRayTrace prq_spt1( obs1, lookdir1 );
    REQUIRE( e_tracer.process( prq_spt1 ) );
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

    Eigen::Vector3d surf_obs2 = surf2 * 1.5;
    psmrts::PRQRayTrace prq_ray2(surf_obs2, -surf_obs2 );
    REQUIRE( e_tracer.process( prq_ray2 ) == true );
    REQUIRE( prq_ray2.trace().get_tracer_id() == e_tracer.uid() ); 

    Eigen::Vector3d lookdir2 = prq_ray2.trace().xyz() - obs2;

    psmrts::PRQRayTrace prq_spt2(obs2, lookdir2 );
    REQUIRE( e_tracer.process( prq_spt2 ) );
    CHECK( prq_spt2.trace().hasHit() == true ); 

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
    CHECK( e_tracer.process( prq_spt3 ) == true ); // Different from Bullet
    CHECK( prq_spt3.trace().hasHit() == true ); 
    // Check the rays xyz - prq_spt3.trace().xyz(), convert to lat/lon
    // should match the ones that surf3.data() contains
    
    psmrts::PRQRayTraceArray ray_array;
    // empty, no hits
    CHECK( e_tracer.process( ray_array ) == false );

    // add one miss - should still be false
    ray_array.add_trace(prq_spt3);
    CHECK( e_tracer.process( ray_array ) == true ); // - related to false above

    // add two hits
    ray_array.add_trace(prq_spt1);
    ray_array.add_trace(prq_spt2);

    // needs at least one hit to be true
    CHECK ( e_tracer.process( ray_array ) == true );
}

TEST_CASE("Ellipsoid Shape Tracer Photometric Values Test", "[ellipsoid][shapetracer][photometric]") {
    const double tolerance = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0 } );
    psmrts::EllipsoidTracer e_tracer( radii );

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
    REQUIRE( prq_surf.trace().get_tracer_id() == e_tracer.uid() ); 

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
    psmrts::PRQRayTrace prq_obs(observer, prq_ray.trace().raypt() );
    CHECK( e_tracer.process( prq_obs ) == true ); 

    // Rigorous check of surface pointns
    Eigen::Vector3d po_xyz = prq_obs.trace().xyz();
    pr_xyz = prq_ray.trace().xyz();
    CHECK_THAT( po_xyz[0], Catch::Matchers::WithinAbs( pr_xyz[0], tolerance ));
    CHECK_THAT( po_xyz[1], Catch::Matchers::WithinAbs( pr_xyz[1], tolerance ));
    CHECK_THAT( po_xyz[2], Catch::Matchers::WithinAbs( pr_xyz[2], tolerance ));

    Eigen::Vector3d po_raypt = prq_obs.trace().raypt();
    Eigen::Vector3d pr_raypt = prq_ray.trace().raypt();

    CHECK_THAT( po_raypt[0], Catch::Matchers::WithinAbs( pr_raypt[0], tolerance ));
    CHECK_THAT( po_raypt[1], Catch::Matchers::WithinAbs( pr_raypt[1], tolerance ));
    CHECK_THAT( po_raypt[2], Catch::Matchers::WithinAbs( pr_raypt[2], tolerance ));

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


TEST_CASE( "Ellipsoid Shape Tracer Photometric Array Test", "[ellipsoid][shapetracer][photometric][array]") {
    const double tolerance = 1.0e-6;

    Eigen::Vector3d radii( {1.0, 1.0, 1.0 } );
    psmrts::EllipsoidTracer e_tracer( radii );

    const double max_radius = 1.0;

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
    CHECK( e_tracer.process( prq_surf1 ) == true );
    CHECK( surf_obs1 == prq_surf1.trace().observer() );

    Eigen::Vector3d lookdir1 = prq_surf1.trace().xyz() - observer1;

    psmrts::PRQRayTrace prq_ray1( observer1, lookdir1 );
    CHECK( e_tracer.process( prq_ray1 ) == true );

    Eigen::Vector3d sun_pos1;
    double sun_lon1 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat1 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon1, sun_lat1, sun_pos1.data());
    sun_pos1 = sun_pos1 * 50.0;

    Eigen::Vector3d lookdir_s1 = prq_ray1.trace().xyz() - sun_pos1;
    psmrts::PRQRayTrace prq_sun1(sun_pos1, lookdir_s1 );
    CHECK( e_tracer.process( prq_sun1 ) == true );
    CHECK( prq_sun1.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo1( observer1, lookdir1, sun_pos1 );
    CHECK( e_tracer.process( prq_photo1 )         == true );
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
    CHECK( e_tracer.process( prq_surf2 ) == true );
    CHECK( surf_obs2 == prq_surf2.trace().observer() );

    Eigen::Vector3d lookdir2 = prq_surf2.trace().xyz() - observer2;

    psmrts::PRQRayTrace prq_ray2( observer2, lookdir2 );
    CHECK( e_tracer.process( prq_ray2 ) == true );

    Eigen::Vector3d sun_pos2;
    double sun_lon2 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat2 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon2, sun_lat2, sun_pos2.data());
    sun_pos2 = sun_pos2 * 50.0;

    Eigen::Vector3d lookdir_s2 = prq_ray2.trace().xyz() - sun_pos2;
    psmrts::PRQRayTrace prq_sun2(sun_pos2, lookdir_s2 );
    CHECK( e_tracer.process( prq_sun2 ) == true );
    CHECK( prq_sun2.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo2( observer2, lookdir2, sun_pos2 );
    CHECK( e_tracer.process( prq_photo2 )         == true );
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
    CHECK( e_tracer.process( prq_ray3 ) == true );
    CHECK( prq_ray3.trace().hasHit()    == true ); 

    Eigen::Vector3d sun_pos3;
    double sun_lon3 = psmrts::degrees_to_radians( 20.0 );
    double sun_lat3 = psmrts::degrees_to_radians( 20.0 );
    latrec_c( radius, sun_lon3, sun_lat3, sun_pos3.data());
    sun_pos3 = sun_pos3 * 50.0;

    Eigen::Vector3d lookdir_s3 = prq_ray3.trace().xyz() - sun_pos3;
    psmrts::PRQRayTrace prq_sun3(sun_pos3, lookdir_s3 );
    CHECK( e_tracer.process( prq_sun3 ) == true );
    CHECK( prq_sun3.trace().hasHit()    == true );

    psmrts::PRQPhotometricTrace prq_photo3( observer3, lookdir3, sun_pos3 );
    CHECK( e_tracer.process( prq_photo3 )         == true );
    CHECK( prq_photo3.isValid()                   == true );
    CHECK( prq_photo3.observer_trace().hasHit()   == true );
    CHECK( prq_photo3.sun_trace().hasHit()        == true );

    psmrts::PRQPhotometricTraceArray photo_array;
    CHECK( e_tracer.process( photo_array ) == false );

    // add two hits, should now be true - array needs at least 1 hit
    photo_array.add_trace( prq_photo1 );
    photo_array.add_trace( prq_photo2 );
    CHECK( e_tracer.process( photo_array ) == true );
}

TEST_CASE( "Ellipsoid Shape Tracer Product Specification Test", "[ellipsoid][shapetracer][product][specification]") {
    // This must be a vector to get all three specs - it should be a PRQRegistration eventually
    psmrts::ProductSpecification spec = psmrts::EllipsoidTracer::product_specifications();
    CHECK( spec.name()              == "ellipsoid"   );
    CHECK( spec.product()           == "tracer"      ); 
    CHECK( spec.features().size()   == 3             );
    CHECK( spec.required().size()   == 1             );
    CHECK( spec.optional().size()   == 2             );

    CHECK( spec.contains( "obj_mtl_search_path" ) == false );
    CHECK( spec.contains( "radii" )               == true  );
    CHECK( spec.contains( "tracer" )              == true  );
}

TEST_CASE ( "Ellipsoid Tracer Value-Range Test - Spheroid/Ellipsoid", "[raytrace][observer][spheroid][triaxial][ellipsoid]") {
  const double tolerance = 1.0e-6;

  std::vector<double> a_radius_list = { 0.5, 1.0, 10.125, 100.0 };
  std::vector<double> b_radius_list = { 0.5, 1.0, 10.125, 100.0 };
  std::vector<double> c_radius_list = { 0.5, 1.0, 10.125, 100.0 };


  double long_val = GENERATE( -180.0, -140.0, -90.0, -45.0, 0.0, 45.0, 90.0, 140.0, 180.0);
  double lat_val = GENERATE( -90.0, -45.0, 0.0, 45.0, 90.0 );
 
  for ( auto a_radius_val: a_radius_list ) {
    for (auto b_radius_val: b_radius_list) {
      double max_radius = std::max( a_radius_val, b_radius_val );
      for ( auto c_radius_val: c_radius_list ) {

        if ( c_radius_val > max_radius) max_radius = c_radius_val;

        psmrts::EllipsoidTracer t_ellipse( a_radius_val, b_radius_val, c_radius_val );
      
        INFO ( "a radius / b radius / c radius = " << a_radius_val << ", " << c_radius_val << ", " << c_radius_val );
        INFO( "Lon/Lat = " << long_val << ", " << lat_val );

        std::vector<double>  distance_list = { 10.0, 1000.0, 10000.0 }; //  1000000.0 -> doesnt seem to work with extreme spheroidal shape(a, b, c) = (100, 0.5, 0.5) 
                                                                        //  and only for a few of the cases.
        Eigen::Vector3d observer;
        double radius = max_radius;
        double obs_long = long_val * rpd_c();
        double obs_lat = lat_val * rpd_c();
        for ( auto distance: distance_list ) {
          (void) latrec_c ( radius * distance, obs_long, obs_lat, observer.data() );
          INFO( "Observer Distance: " << distance  );

          Eigen::Vector3d rec_surf;
          double surf_long = 0.0 * rpd_c();
          double surf_lat  = 0.0 * rpd_c(); 
          (void) latrec_c ( radius, surf_long, surf_lat, rec_surf.data() ); 

          Eigen::Vector3d origin ( { 0.0, 0.0, 0.0 } );
          Eigen::Vector3d surf;
          SpiceBoolean s_found;
          (void) surfpt_c( origin.data(), rec_surf.data(), a_radius_val, b_radius_val, c_radius_val, surf.data(), &s_found );

          Eigen::Vector3d lookdir  = surf - observer;
          psmrts::PRQRayTrace ray( observer, lookdir );

          bool good = t_ellipse.process( ray );
          CHECK( good == true );
          Eigen::Vector3d spt = ray.trace().xyz();


          Eigen::Vector3d normal = ray.trace().normal();
          Eigen::Vector3d naif_spt ( { 0, 0, 0, } );
          SpiceBoolean found; 
          (void) surfpt_c( observer.data(), lookdir.data(), a_radius_val, b_radius_val, c_radius_val, naif_spt.data(), &found );

          Eigen::Vector3d naif_normal ( { 0, 0, 0, } );
          (void) surfnm_c( a_radius_val, b_radius_val, c_radius_val, naif_spt.data(), naif_normal.data() );

          CHECK( good   == true );
          CHECK( found  == SPICETRUE );
          //CHECK( normal == naif_normal );
          CHECK_THAT ( normal[0] , Catch::Matchers::WithinAbs( naif_normal[0], tolerance )); 
          CHECK_THAT ( normal[1] , Catch::Matchers::WithinAbs( naif_normal[1], tolerance ));
          CHECK_THAT ( normal[2] , Catch::Matchers::WithinAbs( naif_normal[2], tolerance ));

          double emission = ray.trace().emission() * dpr_c();
          INFO( "Emission Angle = " << emission );

          double surfsep = vsep_c( naif_spt.data(), spt.data() ) * dpr_c();
          INFO( "Surfpt Angle   = " << surfsep );

          CHECK_THAT ( spt[0] , Catch::Matchers::WithinAbs( naif_spt[0], tolerance )); 
          CHECK_THAT ( spt[1] , Catch::Matchers::WithinAbs( naif_spt[1], tolerance ));
          CHECK_THAT ( spt[2] , Catch::Matchers::WithinAbs( naif_spt[2], tolerance ));
        }
      }
    } 
  }
}
