#include <stdio.h>
#include <string>

#include <PsmrtsRequest.hpp>

//#include <PsmrtsBulletWorldModel.hpp>
//#include <BulletTracerModel.hpp>
//#include <BulletShapeTracer.hpp>
//#include <PsmrtsOBJFormat.hpp>
//#include <PsmrtsUtilities.hpp>
#include <PsmrtsShapeTracer.hpp>

#include <psmrts_c.h>

int main( int argc, char *argv[] ) {

  // retrieve obj file name from argv[1]
  std::string objfile = argv[1];

  // This spec saves significant memory... and confirms Bullet preserves data
  // Create PsmrtsBulletWorldModel from input obj file
  psmrts::bullet::PsmrtsBulletWorldModel *bt_world = 0;
  try {
      bt_world =
          new psmrts::bullet::PsmrtsBulletWorldModel(psmrts::bullet::PsmrtsBulletMeshMap( psmrts::PsmrtsOBJFormat( objfile ) ),
                                                     objfile );
  }
  catch (const std::runtime_error& e) {
      std::cerr << "Runtime error: " << e.what() << std::endl;
  }
  catch (...) {
      std::cerr << "Unknown exception." << std::endl;
  }

  // create Bullet shape tracer from file string & product features request
  // (PRQ) objects
  psmrts::BulletShapeTracer bulletShapeTracer( *bt_world );
  const double max_radius = bt_world->mesh().maximum_radius();
  printf("max radius = %f\n", max_radius);
  psmrts::PRQFeatures features;

  // OK: max radius should be 0.283065

  // validate PRQ
  if (bulletShapeTracer.process(features) != true ) {
      printf("uh oh!\n");
  }

  // validate Base features PRQ functionality before we've done anything
  printf( "PRQ Features Basic functionality\n");
  printf( "        name: %s\n", features.name().c_str() );
  printf( "     invoked: %d\n", features.was_invoked() );
  printf( "   run count: %zu\n", features.run_count() );
  printf( " error count: %zu\n", features.error_count() );
  printf( "  error size: %zu\n", features.errors().size() );

  printf( "\nTest: observer@45,45; surface target@45,50\n");

  // Compute the position of an observer at ( 45,45 ) degrees
  Eigen::Vector3d obs;
  double r = 1.0;
  double obs_lon = 45.0 * rpd_c();
  double obs_lat = 45.0 * rpd_c();
  latrec_c ( r, obs_lon, obs_lat, obs.data() );
  obs = obs * 10.0;

  // OK: obs should be 5.0, 5.0, 7.071068

  // Compute surface point at ( 45, 50 ). This is our surface target
  Eigen::Vector3d surf;
  double surf_lon = 45.0 * rpd_c();
  double surf_lat = 50.0 * rpd_c();
  latrec_c ( r, surf_lon, surf_lat, surf.data() );

  // OK: surf should be 0.454519, 0.454519, 0.766044

  // Find the real surface point using bullet
  Eigen::Vector3d surf_obs = surf * (max_radius + 1.5);
  psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs );

  // validate bullet shape tracer with PRQRayTrace
  if ( bulletShapeTracer.process( prq_ray ) == false ) {
      printf("uh oh! bulletShapeTracer with prq_ray process is false\n");
  }

  // Compute expected/precise look vector from
  // observer to surface intercept point
  Eigen::Vector3d lookdir = prq_ray.trace().xyz() - obs;
  printf("Observer-Surface Intercept Look Vector\n");
  printf("X = %f\nY = %f\nZ = %f\n\n", lookdir[0], lookdir[1], lookdir[2]);

  // OK: lookdir should be -4.885201,-4.885201,-6.877586

  // Trace it from observer to surface point to confirm
  psmrts::PRQRayTrace prq_spt(obs, lookdir );
  bulletShapeTracer.process( prq_spt );

  Eigen::Vector3d normal = prq_spt.trace().normal();
  Eigen::Vector3d xyz = prq_spt.trace().xyz();

  printf("Normal at Observer-Surface Intercept\n");
  printf("X = %f\nY = %f\nZ = %f\n\n", normal[0], normal[1], normal[2]);

  // OK: normal should be 2.59931e-08, 0.525731, 0.850651

  printf("XYZ at Observer-Surface Intercept\n");
  printf("X = %f\nY = %f\nZ = %f\n\n", xyz[0], xyz[1], xyz[2]);

  // OK: xyz should be 0.114799, 0.114799, 0.193482

  // Validate points
  printf( "  prq_ray valid: %d\n", prq_ray.isValid() );
  printf( "  prq_spt valid: %d\n", prq_spt.isValid() );
  printf( "prq_spt has hit: %d\n", prq_spt.trace().hasHit() );

  // Compute radius/lon/lat from intercept surface point (body-fixed)
  double bt_lat, bt_lon, bt_radius;
  reclat_c( xyz.data(), &bt_radius, &bt_lon, &bt_lat);

  // OK: lat= 0.872665; lon= 0.785398; radius= 0.252572

  psmrts::PRQFacet prq_facet( prq_ray.trace() );
  // CHECK( prq_facet.isValid() == true );
  bulletShapeTracer.process( prq_facet );
  // CHECK( prq_facet.facet().isValid() == true );
  // CHECK( prq_facet.prq_trace().emission() == prq_ray.emission() );

  Eigen::Vector3d facet_xyz( prq_facet.trace().xyz() );
  // CHECK_THAT( facet_xyz[0], Catch::Matchers::WithinAbs( xyz[0], tolerance_km));
  // CHECK_THAT( facet_xyz[1], Catch::Matchers::WithinAbs( xyz[1], tolerance_km));
  // CHECK_THAT( facet_xyz[2], Catch::Matchers::WithinAbs( xyz[2], tolerance_km));

  // // get plate id, segment id (may always be 0 in bullet)
  // CHECK( prq_facet.trace().segment_number() == 0 );
  // CHECK( prq_facet.trace().plateid()        == 30 );
  // CHECK( prq_facet.facet().m_indexes[0]     == 11 );
  // CHECK( prq_facet.facet().m_indexes[1]     == 14 );
  // CHECK( prq_facet.facet().m_indexes[2]     == 5 );

  // CHECK_THAT( prq_facet.facet().m_normal[0], Catch::Matchers::WithinAbs( 0.00000002599305449, tolerance_km));
  // CHECK_THAT( prq_facet.facet().m_normal[1], Catch::Matchers::WithinAbs( 0.52573108811158831, tolerance_km));
  // CHECK_THAT( prq_facet.facet().m_normal[2], Catch::Matchers::WithinAbs( 0.85065082318951801, tolerance_km));

  // CHECK_THAT( prq_facet.facet().m_vector1[0], Catch::Matchers::WithinAbs( 0.10100385653540001, tolerance_km ) );
  // CHECK_THAT( prq_facet.facet().m_vector1[1], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
  // CHECK_THAT( prq_facet.facet().m_vector1[2], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );

  // CHECK_THAT( prq_facet.facet().m_vector2[0], Catch::Matchers::WithinAbs( 0.1634276539482 , tolerance_km ) );
  // CHECK_THAT( prq_facet.facet().m_vector2[1], Catch::Matchers::WithinAbs( 0.1634276539482, tolerance_km ) );
  // CHECK_THAT( prq_facet.facet().m_vector2[2], Catch::Matchers::WithinAbs( 0.1634276539482, tolerance_km ) );

  // CHECK_THAT( prq_facet.facet().m_vector3[0], Catch::Matchers::WithinAbs( 0.0, tolerance_km ) );
  // CHECK_THAT( prq_facet.facet().m_vector3[1], Catch::Matchers::WithinAbs( 0.26443149432320001, tolerance_km ) );
  // CHECK_THAT( prq_facet.facet().m_vector3[2], Catch::Matchers::WithinAbs( 0.10100385653540001, tolerance_km ) );

  /*
  // psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
  // psmrts::BulletShapeTracer b_tracer( bt_world );

  // Max radius of object: 0.28306500000006685
//  const double max_radius = bt_world.mesh().maximum_radius();


  PSMRTS_ShapeTracer   *ellipsoid = 0;
  PSMRTS_RayTrace      *ray = 0, *ray2 = 0, *sunray = 0;
  PSMRTS_Vector3d      observer, observer2, lookdir, lookdir2, sunpos, sundir, position_v, look_v;
  PSMRTS_Vector3d      normal, sepang, xyz, raypt, raypt2, radlonlat;
  PSMRTS_Vector3d      obs_t, lkdir_t;
  PSMRTS_Vector3d      v1, v2;

  double slant_d, surft_dist;
  double radius_km;

  #if 0

    // Create an ellipsoid tracer
#if 0
  const char *ellipsoid_s = "tracer=ellipsoid;radii=[0.283065,0.271215,0.249720]";
  ellipsoid = psmrts_create_tracer_from_string( p_factory, ellipsoid_s );
#else
  const char *tracer = "tracer=ellipsoid";
  const char *shape = "ellipsoid=0.283065,0.271215,0.249720";
  ellipsoid = psmrts_load_shape( shape, tracer );
#endif
  if ( !psmrts_tracer_valid( ellipsoid ) )  ) {
    printf("\n*** PSMRTS-C - create errors:\n%s\n", psmrts_tracer_error_str( ellipsoid ) );
    exit ( 1 );
  }
#endif
  // implement method in line 43, ellipsoid = ...

  // create Bullet tracer
  // put in routine that returns
  // psmrts_create_ellipsoid(double, double, double, char* ); pass in 0 for pointer
  // bullet, etc.
//  psmrts::PsmrtsShapeTracer bullet_t(psmrts::PsmrtsShapeTracer::bullet(objfile) );
// create with string

  // Trace a ray
  // create observer at lat, lon of 45, 45 degrees
  observer  = psmrts_vector3d( 1.0, 1.0, 1.0 );

  observer2 = psmrts_vector3d( 1000.0, 1000.4, 1000.0 );
  lookdir   = psmrts_negate( observer );
  lookdir2  = psmrts_negate( observer2 );

  // Trace a ray on the ellipsoid. Create a reusable ray structure
  // to minimize memory create/free overhead.
  ray = psmrts_create_ray( observer, lookdir );
  ray2 = psmrts_create_ray( observer2, lookdir2 );
  // ray = psmrts_ray_trace( ray, ellipsoid );
  // if ( psmrts_ray_has_hit( ray ) ) {
  printf("\n*** PSMRTS-C - Trace from ellipsoid succeeded!\n");

  // Do rays intercept ellipsoid?
  if (psmrts_ray_has_hit(ray)) {
      printf("ray HAS hit\n");
  }
  else {
      printf("ray HAS NOT hit\n");
  }

  if (psmrts_ray_has_hit(ray)) {
      printf("ray2 HAS hit\n");
  }
  else {
      printf("ray2 HAS NOT hit\n");
  }

  // Retrieve/calculate data from trace
  obs_t     = psmrts_ray_observer( ray );
  printf("\nObserver: %f %f %f\n", obs_t.x, obs_t.y, obs_t.z );

  obs_t     = psmrts_ray_observer( ray2 );
  printf("\nObserver2: %f %f %f\n", obs_t.x, obs_t.y, obs_t.z );

  lkdir_t   = psmrts_ray_lookdir( ray );
  printf("Lookdir:  %f %f %f\n", lkdir_t.x, lkdir_t.y, lkdir_t.z );

  lkdir_t   = psmrts_ray_lookdir( ray2 );
  printf("Lookdir2:  %f %f %f\n", lkdir_t.x, lkdir_t.y, lkdir_t.z );

  raypt   = psmrts_ray_raypt( ray );
  printf("SurfPt:  %f %f %f\n", raypt.x, raypt.y, raypt.z );

  raypt2   = psmrts_ray_raypt( ray2 );
  printf("RayPt:  %f %f %f\n", raypt2.x, raypt2.y, raypt2.z );

  double radius = psmrts_ray_intercept_radius(ray);
  printf("intercept radius:  %f\n", radius );

  double slant_distance = psmrts_ray_intercept_slant_distance(ray);
  printf("intercept slant distance:  %f\n", slant_distance );

  slant_distance = psmrts_ray_intercept_slant_distance(ray2);
  printf("intercept2 slant distance:  %f\n", slant_distance );

  // compute distance between ray and ray2 surface intercepts
  double interceptsDistance
      = psrmrts_ray2ray_distance(ray, ray2);
  printf("distance between intercepts:  %f\n", interceptsDistance );

  // compute separation angle between two vectors
  double separationAngleRadians
      = psmrts_separation_angle_radians(&observer, &observer2);
  printf("separation angle between vectors (radians):  %f\n", separationAngleRadians );

  // determine if two rays are near one another based on given tolerance
  bool near = psrmrts_isNear(ray, ray2, 0.001);
  if ( near )
      printf("rays ARE near\n");
  else
      printf("rays ARE NOT near\n");

  // compute incidence angle between ray and ray2
  double incidenceAngle = psmrts_incidence(ray, ray2);
  printf("incidence angle:  %f\n", incidenceAngle );

  // compute emission angle at ray surface point intercept
  double emissionAngle = psmrts_emission(ray);
  printf("emission angle:  %f\n", emissionAngle );

  // compute phase angle from a surface point (lookdirs) to two observer positions
  double phaseAngle = psmrts_phase(ray, ray2);
  printf("phase angle:  %f\n", phaseAngle );

  // Use sunpos to get observational geometry
  sunpos = psmrts_vector3d( 300, 1000, 2000 );
  sundir = psmrts_subtract( psmrts_ray_observer( ray ), sunpos );

#if 0    
    // Trace from sun position to surface intercept point
    sunray = psmrts_ray_trace( psmrts_create_ray( sunpos, sundir ), ellipsoid );
    if ( psmrts_ray_has_hit( sunray ) ) {
      printf("\n*** PSMRTS-C - Trace from sun succeeded!\n");
    }
    else {
      printf("\n*** PSMRTS-C - Trace from sun failed!\n");
    }
#endif    
  // }

  // create photometric ray traces
  PSMRTS_PhotometricRayTrace *photoRay1
      = psmrts_create_photometric_ray( observer, lookdir, sunpos );
  PSMRTS_PhotometricRayTrace *photoRay2
      = psmrts_create_photometric_ray( observer2, lookdir2, sunpos );

  printf("\n*** PSMRTS-C - Photometric Trace Info \n");

  // photometric angles
  double photoemission1 = psmrts_photo_emission( photoRay1 );
  double photoemission2 = psmrts_photo_emission( photoRay2 );
  printf( "photometric emission angle1:  %f\n", photoemission1 );
  printf( "photometric emission angle2:  %f\n", photoemission2 );

  double photoPhaseAngle = psmrts_photo_phase( photoRay1, photoRay2 );
  printf( "photometric phase angle:  %f\n", photoPhaseAngle );

  // Resource cleanup and factory shutdown
  psmrts_free_ray( ray );
  psmrts_free_ray( sunray );
  // psmrts_free_tracer( ellipsoid );
*/
  if (bt_world != NULL) {
      delete bt_world;
  }

  return ( 0 );
}
