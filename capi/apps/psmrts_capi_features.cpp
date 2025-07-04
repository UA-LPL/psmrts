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

    // emulating catch2 "Bullet Shape Tracer Ray Trace Array Test"
  std::string objfile =
        "/Users/kledmundson/ISISDev/UA-LPL/June232025/psmrts/formats/obj/data/bennu_20facets.obj";

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
  psmrts::PsmrtsShapeTracer bullet_t(psmrts::PsmrtsShapeTracer::bullet(objfile) );
// create with string

  // Trace a ray
  observer  = psmrts_vector3d( 0.3, 0.0, 0.0 );
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

  return ( 0 );
}
