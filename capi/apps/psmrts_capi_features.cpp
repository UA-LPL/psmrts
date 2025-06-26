#include <stdio.h>
#include <psmrts_c.h>

int main( int argc, char *argv[] ) {

  PSMRTS_ShapeTracer   *ellipsoid = 0;
  PSMRTS_RayTrace      *ray= 0, *sunray = 0;
  PSMRTS_Vector3d      observer, lookdir, sunpos, sundir, position_v, look_v;
  PSMRTS_Vector3d      emission, incidence, phase, normal, sepang, xyz, surfpt, radlonlat;

  PSMRTS_Vector3d      obs_t, lkdir_t;

  double slant_d, surft_dist;
  double radius_km;

  #if 0

    /* Create an ellipsoid tracer */
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

  /** Trace a ray */
  observer = psmrts_vector3d( 0.3, 0.0, 0.0 );
  lookdir  = psmrts_negate( observer );

  /* Trace a ray on the ellipsoid. Create a reusable ray structure  */
  /* to minimize memory create/free overhead. */
  ray = psmrts_create_ray( observer, lookdir );
  // ray = psmrts_ray_trace( ray, ellipsoid );
  // if ( psmrts_ray_has_hit( ray ) ) {
      printf("\n*** PSMRTS-C - Trace from ellipsoid succeeded!\n");

  /* Retrieve/calculate data from trace */
  obs_t     = psmrts_ray_observer( ray );
  printf("\nObserver: %f %f %f\n", obs_t.x, obs_t.y, obs_t.z );

  lkdir_t   = psmrts_ray_lookdir( ray );
  printf("Lookdir:  %f %f %f\n", lkdir_t.x, lkdir_t.y, lkdir_t.z );

  double radius = psmrts_ray_intercept_radius(ray);
  printf("intercept radius:  %f\n", radius );

  double slant_distance = psmrts_ray_intercept_slant_distance(ray);
  printf("intercept slant distance:  %f\n", slant_distance );

  /* Use sunpos to get observational geometry */
  sunpos = psmrts_vector3d( 300, 1000, 2000 );
  sundir = psmrts_subtract( psmrts_ray_observer( ray ), sunpos );

#if 0    
    /* Trace from sun position to surface intercept point */
    sunray = psmrts_ray_trace( psmrts_create_ray( sunpos, sundir ), ellipsoid );
    if ( psmrts_ray_has_hit( sunray ) ) {
      printf("\n*** PSMRTS-C - Trace from sun succeeded!\n");
    }
    else {
      printf("\n*** PSMRTS-C - Trace from sun failed!\n");
    }
#endif    
  // }

  /* Resource cleanup and factory shutdown */
  psmrts_free_ray( ray );
  psmrts_free_ray( sunray );
  // psmrts_free_tracer( ellipsoid );

  return ( 0 );
}
