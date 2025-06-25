#include <stdio.h>
#include <psmrts_c.h>

int main( int argc, char *argv[] ) {

  PSMRTS_ShapeTracer   *ellipsoid;
  PSMRTS_Ray           *ray, *sunray;
  PSMRTS_Vector3d      observer, lookdir, sunpos, sundir, position_v, look_v;
  PSMRTS_Vector3d      emission, incidence, phase, normal, sepang, xyz, surfpt, radlonlat;

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
  if ( psmrts_ray_has_hit( ray ) ) {
      printf("\n*** PSMRTS-C - Trace from ellipsoid succeeded!\n");

    /* Retrieve/calculate data from trace */
    xyz       = psmrts_ray_xyz( ray );
    normal    = psmrts_ray_normal( ray );

    /* Use sunpos to get observational geometry */
    sunpos = psmrts_vector3d( 300, 1000, 2000 );
    sundir = psmrts_subtract( psmrts_ray_xyz( ray ), sunpos );

    /* Trace from sun position to surface intercept point */
    // sunray = psmrts_ray_trace( psmrts_create_ray( sunpos, sundir ), ellipsoid );
    if ( psmrts_ray_has_hit( sunray ) ) {
      printf("\n*** PSMRTS-C - Trace from sun succeeded!\n");
    }
    else {
      printf("\n*** PSMRTS-C - Trace from sun failed!\n");
    }
  }

  /* Resource cleanup and factory shutdown */
  psmrts_free_ray( ray );
  psmrts_free_ray( sunray );
  // psmrts_free_tracer( ellipsoid );

  return ( 0 );
}