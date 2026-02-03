/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psmrts/capi/psmrts_c.h>

int main( int argc, char *argv[] ) {

    if ( argc < 2 ){
        fprintf(stderr, "Error: filepath is required as an argument.\n");
        return (1);
    }

    // Determine filetype
    const char* file = argv[1];
    char* filetype = strrchr( file, '.' );
    PSMRTS_Tracer *tracer;

    if ( strcmp(filetype, ".obj" ) == 0 || strcmp(filetype, ".ply" ) == 0 ) {
        tracer = psmrts_create_bullet(file);
    }
    else if ( strcmp(filetype, ".bds" ) == 0 ) {
        tracer = psmrts_create_naifdsk(file);
    }
    else {
        fprintf(stderr, "Error: file type is required to be .bds, .obj, or .ply.\n");
        return (1);
    }

    // Validate
    if ( !psmrts_tracer_valid(tracer) ) {
        fprintf(stderr, " *** PSMRTS Error: bullet tracer failed to load - %s *** \n", file);
        exit(1);
    }

    PSMRTS_Vector3d xyz_d, raypt_d, normal_d, llr_d;
    double slant_d, raypt_d2, radius_pt;

    const char* version = psmrts_version();
    printf("***** PSMRTS | Version: %s ******\n", version);

    char* result = strrchr(argv[1], '/');
    printf("Tracer Successfully Built: %s\n", result);

    printf("\n *** Creating Ray Trace ***\n");
    PSMRTS_Vector3d obs = psmrts_vector3d( 45.0, 45.0, 1.0 );
    obs = psmrts_lonlatrad_to_xyz_d(&obs);
    obs = psmrts_scale(&obs, 10.0); // set point to the 10 km loc
    printf("Observer: {%f, %f, %f}   |   lon: 45.0°, lat: 45.0°, radius: 10.0km \n", obs.x, obs.y, obs.z);
    
    // Calculate a desired surf point target from 45.0°|50.0°
    PSMRTS_Vector3d surf = psmrts_vector3d(45.0, 50.0, 1.0);
    surf = psmrts_lonlatrad_to_xyz_d(&surf);
    surf = psmrts_scale(&surf, 1.5);
    PSMRTS_Vector3d surf_neg = psmrts_negate(&surf);

    PSMRTS_RayTrace *surf_ray = psmrts_ray_trace_v(&surf, &surf_neg, tracer);
    PSMRTS_Vector3d surf_xyz = psmrts_ray_xyz(surf_ray);
    double x = surf_xyz.x - obs.x;
    double y = surf_xyz.y - obs.y;
    double z = surf_xyz.z - obs.z;
    PSMRTS_Vector3d lkdr = psmrts_vector3d(x, y, z);
    printf("Look Direction: {%f, %f, %f}\n", lkdr.x, lkdr.y, lkdr.z);

    printf("Running Trace...\n");
    PSMRTS_RayTrace *ray = psmrts_ray_trace_v(&obs, &lkdr, tracer);
    PSMRTS_BOOL status = psmrts_ray_has_hit(ray);

    if (status) {
        printf("Ray Hit Confirmed [✓] \n");
    }
    else {
        printf("Ray Missed [x] \n");
        return 0;
    }

    printf(" --- Ray Results --- \n");
    PSMRTS_Vector3d xyz = psmrts_ray_xyz( ray );
    printf("xyz: {%f, %f, %f}\n", xyz.x, xyz.y, xyz.z);

    PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
    printf("raypt: {%f, %f, %f}\n", raypt.x, raypt.y, raypt.z);

    PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
    printf("normal: {%f, %f, %f}\n", normal.x, normal.y, normal.z );

    double intercept_radius = psmrts_ray_intercept_radius( ray ); 
    printf("intercept radius: %f \n", intercept_radius);

    double intercept_slant = psmrts_ray_intercept_slant_distance( ray );
    printf("intercept slant distance: %f \n", intercept_slant);

    printf("\n *** Creating Photometric Ray Trace *** \n");
    PSMRTS_Vector3d sun_pos = psmrts_vector3d( 30000, 100000, 200000 );
    printf("Sun Position: {%f, %f, %f}\n", sun_pos.x, sun_pos.y, sun_pos.z);

    PSMRTS_Vector3d sun_dir = psmrts_subtract(&xyz, &sun_pos);
    printf("Sun Direction: {%f, %f, %f} \n", sun_dir.x, sun_dir.y, sun_dir.z);

    printf("Running Photometric trace..\n");
    PSMRTS_PhotometricRayTrace *p_ray = psmrts_create_photometric_ray(&obs, &lkdr, &sun_pos);
    p_ray = psmrts_photo_ray_trace(p_ray, tracer);
    
    printf(" --- Photometric Ray Results (radians) --- \n");
    double emission = psmrts_photometric_emission(p_ray);
    printf("Emission: %f \n", emission);
    double incidence = psmrts_photometric_incidence(p_ray);
    printf("Incidence: %f \n", incidence);
    double phase = psmrts_photometric_phase(p_ray);
    printf("Phase: %f \n", phase);

    printf("\n *** Creating Ray Trace Array *** \n");
    PSMRTS_TraceArray *t_array = psmrts_create_trace_array();
    printf("Creating Rays:\n");

    PSMRTS_Vector3d obs1 = psmrts_vector3d( 75.0, 35.0, 1.0 );
    obs1 = psmrts_lonlatrad_to_xyz_d(&obs1);
    obs1 = psmrts_scale(&obs1, 10.0); // set point to the 10 km loc
    printf("1. Observer: {%f, %f, %f}   |   lon: 75.0°, lat: 35.0°, radius: 10.0km \n", obs1.x, obs1.y, obs1.z);
    PSMRTS_Vector3d lkdr1 = psmrts_negate( &obs1 );
    printf("   Look Direction: {%f, %f, %f}\n", lkdr1.x, lkdr1.y, lkdr1.z);
    PSMRTS_RayTrace *ray1 = psmrts_create_ray( &obs1, &lkdr1 );
    psmrts_trace_array_add_trace( t_array, ray1 );

    PSMRTS_Vector3d obs2 = psmrts_vector3d( 60.0, 125.0, 1.0 );
    obs2 = psmrts_lonlatrad_to_xyz_d(&obs2);
    obs2 = psmrts_scale(&obs2, 10.0); // set point to the 10 km loc
    printf("2. Observer: {%f, %f, %f}   |   lon: 60.0°, lat: 125.0°, radius: 10.0km \n", obs1.x, obs1.y, obs1.z);
    PSMRTS_Vector3d lkdr2 = psmrts_negate( &obs2 );
    printf("   Look Direction: {%f, %f, %f}\n", lkdr2.x, lkdr2.y, lkdr2.z);
    PSMRTS_RayTrace *ray2 = psmrts_create_ray( &obs2, &lkdr2 );
    psmrts_trace_array_add_trace( t_array, ray2 );

    PSMRTS_Vector3d obs3 = psmrts_vector3d( 20.0, 10.0, 10.0 );
    obs3 = psmrts_lonlatrad_to_xyz_d(&obs3);
    obs3 = psmrts_scale(&obs3, 10.0); // set point to the 10 km loc
    printf("3. Observer: {%f, %f, %f}   |   lon: 20.0°, lat: 10.0°, radius: 100.0km \n", obs1.x, obs1.y, obs1.z);
    PSMRTS_Vector3d lkdr3 = psmrts_negate( &obs3 );
    printf("   Look Direction: {%f, %f, %f}\n", lkdr3.x, lkdr3.y, lkdr3.z);
    PSMRTS_RayTrace *ray3 = psmrts_create_ray( &obs3, &lkdr3 );
    psmrts_trace_array_add_trace( t_array, ray3 );

    psmrts_trace_array_trace( t_array, tracer );
    int ntraces = psmrts_trace_array_size( t_array );
    for (int i = 0; i < ntraces; i++) {
        const PSMRTS_RayTrace *result = psmrts_trace_array_get_trace( t_array, i );
        printf("Ray %i: ", i+1);
        if ( psmrts_ray_has_hit( result ) == PSMRTS_FALSE ) {
            printf("Ray Missed [x] \n ");
        }
        else {
            printf("Ray Hit Confirmed [✓] \n");
            xyz_d        = psmrts_ray_xyz( result );             // intercept in x,y,z
            llr_d        = psmrts_xyz_to_lonlatrad_d( &xyz_d ); // intercept in lon, lat, radius
            raypt_d      = psmrts_ray_raypt( result );
            normal_d     = psmrts_ray_normal( result );
            slant_d    = psmrts_ray_intercept_slant_distance( result ); // should all be zero because they're all negated?
            raypt_d2    = psmrts_length( &raypt_d);
            radius_pt  = psmrts_length( &xyz_d );

            printf( "\n              trace surface intercept (xyz):  %lf,  %lf,  %lf\n", xyz_d.x, xyz_d.y, xyz_d.z ) ;
            printf( "              trace surface intercept (llr):  %lf,  %lf,  %lf\n", llr_d.x, llr_d.y, llr_d.z ) ;
            printf( "          vector along look direction (xyz): %lf, %lf, %lf\n", raypt_d.x, raypt_d.y, raypt_d.z ) ;
            printf( "   normal vector at surface intercept (xyz):  %lf,  %lf,  %lf\n", normal_d.x, normal_d.y, normal_d.z );
            printf( "        slant distance at surface intercept:  %lf\n", slant_d );
            printf( " length of look direction vector to surface:  %lf\n", raypt_d2 );
            printf( "    target radius at surface intercept (km):  %lf\n", radius_pt );
        }
        printf("\n");
    }

    psmrts_free_ray( ray3 );
    psmrts_free_ray( ray2 );
    psmrts_free_ray( ray1 );
    psmrts_free_trace_array(t_array);
    psmrts_free_photometric_ray(p_ray);
    psmrts_free_ray(ray);
    psmrts_free_tracer( tracer );

    return 0;
}
