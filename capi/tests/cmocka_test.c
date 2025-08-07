#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <cmocka.h>

#include "psmrts_c.h" 

const double tolerance = 1e-12;

// --- PSMRTS information Functions ---
static void test_psmrts_version_info(void **state) {
    (void)state;

    const char* version = psmrts_version();
    const char* info = psmrts_info();

    assert_string_equal(version, "0.2.1");
    assert_string_equal(info, "psmrts-0.2.1");
}

// --- PSMRTS Vector3d Functions ---
static void test_psmrts_vector3d_init(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 2.0, 3.0);

    assert_double_equal(v.x, 1.0, tolerance);
    assert_double_equal(v.y, 2.0, tolerance);
    assert_double_equal(v.z, 3.0, tolerance);
}

static void test_psmrts_negate(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(2.0, 3.0, 4.0);
    PSMRTS_Vector3d v_neg = psmrts_negate( &v );

    assert_double_equal(v_neg.x, -2.0, tolerance);
    assert_double_equal(v_neg.y, -3.0, tolerance);
    assert_double_equal(v_neg.z, -4.0, tolerance);

    PSMRTS_Vector3d v2 = psmrts_vector3d(2.0, -3.0, 4.0);
    PSMRTS_Vector3d v2_neg = psmrts_negate( &v2 );

    assert_double_equal(v2_neg.x, -2.0, tolerance);
    assert_double_equal(v2_neg.y, 3.0, tolerance);
    assert_double_equal(v2_neg.z, -4.0, tolerance);
}

static void test_psmrts_subtract(void **state) {
    (void)state;

    PSMRTS_Vector3d v1 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d v2 = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d result = psmrts_subtract(&v1, &v2);

    assert_double_equal(result.x, 3.0, tolerance);
    assert_double_equal(result.y, 3.0, tolerance);
    assert_double_equal(result.z, 3.0, tolerance);

    PSMRTS_Vector3d v3 = psmrts_vector3d(0.0, 10.0, -4.0);
    PSMRTS_Vector3d v4 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d result2 = psmrts_subtract(&v3, &v4);

    assert_double_equal(result2.x, -4.0, tolerance);
    assert_double_equal(result2.y, 5.0, tolerance);
    assert_double_equal(result2.z, -10.0, tolerance);
}

static void test_psmrts_add(void **state) {
    (void)state;

    PSMRTS_Vector3d v1 = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d v2 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d result = psmrts_add(&v1, &v2);

    assert_double_equal(result.x, 5.0, tolerance);
    assert_double_equal(result.y, 7.0, tolerance);
    assert_double_equal(result.z, 9.0, tolerance);
}

static void test_psmrts_scale(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d result = psmrts_scale(&v, 2);

    assert_double_equal(result.x, 2.0, tolerance);
    assert_double_equal(result.y, 4.0, tolerance);
    assert_double_equal(result.z, 6.0, tolerance);
}

static void test_psmrts_length(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 4.0, 8.0);
    double result = psmrts_length(&v);

    assert_double_equal(result, 9.0, tolerance);
}

// --- PSMRTS Trace Functions ---
static void test_psmrts_ray(void **state) {
    (void)state;

    PSMRTS_Vector3d obs = psmrts_vector3d(2.0, 4.0, 6.0);
    PSMRTS_Vector3d lkdr = psmrts_vector3d(-1.0, 0.0, 12.0);

    PSMRTS_RayTrace *ray = psmrts_create_ray(&obs, &lkdr);

    assert_non_null(ray);

    PSMRTS_Vector3d obs_result = psmrts_ray_observer(ray);
    PSMRTS_Vector3d lkdr_result = psmrts_ray_lookdir(ray);

    assert_double_equal(obs_result.x, 2.0, tolerance);
    assert_double_equal(obs_result.y, 4.0, tolerance);
    assert_double_equal(obs_result.z, 6.0, tolerance);

    assert_double_equal(lkdr_result.x, -1.0, tolerance);
    assert_double_equal(lkdr_result.y, 0.0, tolerance);
    assert_double_equal(lkdr_result.z, 12.0, tolerance);

    PSMRTS_BOOL nohit = psmrts_ray_has_hit(ray);

    assert_int_equal(nohit, 0);
    
    PSMRTS_Vector3d raypt = psmrts_ray_raypt(ray);
    assert_double_equal(raypt.x, -2.0, tolerance);
    assert_double_equal(raypt.y, -4.0, tolerance);
    assert_double_equal(raypt.z, -6.0, tolerance);    

    PSMRTS_Vector3d new_obs = psmrts_vector3d(3.0, -5.0, 7.0);
    PSMRTS_Vector3d new_lkdr = psmrts_vector3d(100.0, 20.2, 30.0);

    psmrts_ray_set_observation(&new_obs, &new_lkdr, ray);

    obs_result = psmrts_ray_observer(ray);
    lkdr_result = psmrts_ray_lookdir(ray);

    assert_double_equal(obs_result.x, 3.0, tolerance);
    assert_double_equal(obs_result.y, -5.0, tolerance);
    assert_double_equal(obs_result.z, 7.0, tolerance);

    assert_double_equal(lkdr_result.x, 100.0, tolerance);
    assert_double_equal(lkdr_result.y, 20.2, tolerance);
    assert_double_equal(lkdr_result.z, 30.0, tolerance);

    psmrts_free_ray( ray );
}

static void test_psmrts_raytrace(void **state) {
    (void)state;
    const char *name = "test";
    PSMRTS_Tracer *sphere = psmrts_create_sphere(1.0, name);

    PSMRTS_Vector3d obs = psmrts_vector3d(0.0, 0.0, 3.0);
    PSMRTS_Vector3d lkdr = psmrts_vector3d(0.0, -0.2, -1.0);
    PSMRTS_RayTrace *ray = psmrts_create_ray(&obs, &lkdr);

    ray = psmrts_ray_trace(ray, sphere);

    PSMRTS_BOOL hashit = psmrts_ray_has_hit(ray);
    assert_int_equal(hashit, 1);

    PSMRTS_Vector3d xyz = psmrts_ray_xyz(ray);
    assert_double_equal(xyz.x, 0.0, tolerance);
    assert_double_equal(xyz.y, -0.418342091322, tolerance);
    assert_double_equal(xyz.z, 0.908289543388, tolerance);

    PSMRTS_Vector3d raypt = psmrts_ray_raypt(ray);
    assert_double_equal(raypt.x, 0.0, tolerance);
    assert_double_equal(raypt.y, -0.418342091322, tolerance);
    assert_double_equal(raypt.z, -2.091710456612, tolerance);

    PSMRTS_Vector3d normal = psmrts_ray_normal(ray);
    assert_double_equal(normal.x, 0.0, tolerance);
    assert_double_equal(normal.y, -0.418342091322, tolerance);
    assert_double_equal(normal.z, 0.908289543388, tolerance);

    double intercept = psmrts_ray_intercept_radius(ray);
    assert_double_equal(intercept, 1.0, tolerance);

    double slant = psmrts_ray_intercept_slant_distance(ray);
    assert_double_equal(slant, 2.133134487010, tolerance);

    // Second Ray Trace
    PSMRTS_Vector3d obs2 = psmrts_vector3d(0.0, 2.0, 4.0);
    PSMRTS_Vector3d lkdr2 = psmrts_vector3d(-0.2, -2.0, -2.0);
    PSMRTS_RayTrace *ray2 = psmrts_create_ray(&obs, &lkdr);

    ray2 = psmrts_ray_trace(ray2, sphere);
    PSMRTS_BOOL hashit2 = psmrts_ray_has_hit(ray2);
    assert_int_equal(hashit2, 1); 

    double dist = psmrts_ray2ray_distance(ray, ray2);
    assert_double_equal(dist, 0.0, tolerance); // Should this be 0?

    PSMRTS_Vector3d v1 = psmrts_vector3d(1.0, 0.0, 0.0);
    PSMRTS_Vector3d v2 = psmrts_vector3d(1.0, 1.0, 0.0); 
    double sep_angle = psmrts_separation_angle_radians(&v1, &v2);
    assert_double_equal(sep_angle, 0.785398163397, tolerance);

    PSMRTS_BOOL near = psmrts_isNear(ray, ray2, 5.0);
    assert_int_equal(near, 1);

    double incidence = psmrts_incidence(ray, ray2);
    assert_double_equal(incidence, 0.629014802427, tolerance);

    double emission = psmrts_emission(ray);
    assert_double_equal(emission, 0.629014802427, tolerance);

    double phase = psmrts_phase(ray, ray2);
    assert_double_equal(phase, 0.0, tolerance);

    psmrts_free_tracer( sphere );
    psmrts_free_ray( ray );
    psmrts_free_ray( ray2 );
}

// --- PSMRTS TraceArray Functions ---
static void test_psmrts_trace_array(void **state) {
    (void)state;

    PSMRTS_TraceArray *t_array = psmrts_create_trace_array();
    int size = psmrts_trace_array_size(t_array);
    assert_int_equal(size, 0);

    const char *name = "test";
    PSMRTS_Tracer *ellipse = psmrts_create_sphere(1.0, name);

    PSMRTS_Vector3d obs = psmrts_vector3d(0.0, 0.0, 3.0);
    PSMRTS_Vector3d lkdr = psmrts_vector3d(0.0, 0.0, -1.0);
    PSMRTS_RayTrace *ray = psmrts_create_ray(&obs, &lkdr);

    ray = psmrts_ray_trace(ray, ellipse);

    psmrts_trace_array_add_trace(t_array, ray);
    size = psmrts_trace_array_size(t_array);
    assert_int_equal(size, 1);

    PSMRTS_Vector3d obs2 = psmrts_vector3d(0.0, 3.0, 2.0);
    PSMRTS_Vector3d lkdr2 = psmrts_vector3d(0.0, -2.0, -1.0);
    PSMRTS_RayTrace *ray2 = psmrts_create_ray(&obs2, &lkdr2);

    ray2 = psmrts_ray_trace(ray2, ellipse);

    psmrts_trace_array_add_trace(t_array, ray2);
    size = psmrts_trace_array_size(t_array);
    assert_int_equal(size, 2);

    const PSMRTS_RayTrace *first = psmrts_trace_array_get_trace(t_array, 0);
    PSMRTS_Vector3d first_obs = psmrts_ray_observer(first);
    assert_double_equal(first_obs.x, 0.0, tolerance);
    assert_double_equal(first_obs.y, 0.0, tolerance);
    assert_double_equal(first_obs.z, 3.0, tolerance);

    const PSMRTS_RayTrace *second = psmrts_trace_array_get_trace(t_array, 1);
    PSMRTS_Vector3d sec_obs = psmrts_ray_observer(second);
    assert_double_equal(sec_obs.x, 0.0, tolerance);
    assert_double_equal(sec_obs.y, 3.0, tolerance);
    assert_double_equal(sec_obs.z, 2.0, tolerance);

    psmrts_free_ray( ray2 );
    psmrts_free_ray( ray );
    psmrts_free_tracer( ellipse );
    psmrts_free_trace_array( t_array);
}

// --- PSMRTS Photometric Trace/Array Functions ---
static void test_psmrts_photometric_trace(void **state) {
    (void)state;

    const char *name = "test";
    PSMRTS_Tracer *ellipse = psmrts_create_sphere(1.0, name);

    PSMRTS_Vector3d obs = psmrts_vector3d(45.0, 45.0, 1.0);
    obs = psmrts_lonlatrad_to_xyz_d(&obs);
    obs = psmrts_scale(&obs, 10.0);

    PSMRTS_Vector3d surf = psmrts_vector3d(45.0, 50.0, 1.0);
    surf = psmrts_lonlatrad_to_xyz_d(&surf);
    surf = psmrts_scale(&surf, 1.5);
    PSMRTS_Vector3d surf_neg = psmrts_negate(&surf);

    PSMRTS_RayTrace *surf_ray = psmrts_ray_trace_v(&surf, &surf_neg, ellipse);
    PSMRTS_Vector3d surf_xyz = psmrts_ray_xyz(surf_ray);
    double x = surf_xyz.x - obs.x;
    double y = surf_xyz.y - obs.y;
    double z = surf_xyz.z - obs.z;
    PSMRTS_Vector3d lkdr = psmrts_vector3d(x, y, z);

    PSMRTS_RayTrace *observer_ray = psmrts_ray_trace_v(&obs, &lkdr, ellipse);
    PSMRTS_BOOL obs_hit = psmrts_ray_has_hit(observer_ray);
    assert_int_equal(obs_hit, 1);

    PSMRTS_Vector3d sun_pos = psmrts_vector3d(20.0, 20.0, 1.0);
    sun_pos = psmrts_lonlatrad_to_xyz_d(&sun_pos);
    sun_pos = psmrts_scale(&sun_pos, 50.0);

    PSMRTS_PhotometricRayTrace *p_ray = psmrts_create_photometric_ray(&obs, &lkdr, &sun_pos);

    PSMRTS_PhotometricRayTrace *p_trace = psmrts_photo_ray_trace(p_ray, ellipse);

    double incidence = psmrts_photometric_incidence(p_ray);
    incidence = psmrts_radians_to_degrees(incidence);
    assert_double_equal(incidence, 36.64334758469323816, tolerance);

    double emission = psmrts_photometric_emission(p_ray);
    emission = psmrts_radians_to_degrees(emission);
    assert_double_equal(emission, 5.55459887153097576, tolerance);

    double phase = psmrts_photometric_phase(p_ray);
    phase = psmrts_radians_to_degrees(phase);
    assert_double_equal(phase, 32.73787834081892356, tolerance);

    const PSMRTS_RayTrace *obs_ray = psmrts_photometric_observer_trace(p_ray);
    PSMRTS_Vector3d obs_result = psmrts_ray_observer(obs_ray);
    assert_double_equal(obs_result.x, obs.x, tolerance);
    assert_double_equal(obs_result.y, obs.y, tolerance);
    assert_double_equal(obs_result.z, obs.z, tolerance);

    const PSMRTS_RayTrace *sun_ray = psmrts_photometric_sun_trace(p_trace);
    PSMRTS_Vector3d sun_result = psmrts_ray_observer(sun_ray);
    assert_double_equal(sun_result.x, sun_pos.x, 1e-6);
    assert_double_equal(sun_result.y, sun_pos.y, 1e-6);
    assert_double_equal(sun_result.z, sun_pos.z, 1e-6);

    PSMRTS_Vector3d new_obs = psmrts_vector3d(2.0, 4.0, 6.0);
    PSMRTS_Vector3d new_lkdr = psmrts_negate(&new_obs);

    psmrts_photometric_ray_set_observation(&new_obs, &new_lkdr, &sun_pos, p_ray);
    const PSMRTS_RayTrace *new_obs_ray = psmrts_photometric_observer_trace(p_ray);
    PSMRTS_Vector3d new_obs_result = psmrts_ray_observer(new_obs_ray);
    assert_double_equal(new_obs_result.x, new_obs.x, tolerance);
    assert_double_equal(new_obs_result.y, new_obs.y, tolerance);
    assert_double_equal(new_obs_result.z, new_obs.z, tolerance);

    psmrts_free_tracer( ellipse );
    psmrts_free_ray( observer_ray );
    psmrts_free_photometric_ray( p_ray );
}

static void test_psmrts_photometric_array(void **state) {
    (void)state;

    PSMRTS_PhotometricTraceArray *p_array = psmrts_create_photometric_trace_array();
    int size = psmrts_photometric_trace_array_size(p_array);
    assert_int_equal(size, 0);

    PSMRTS_Vector3d sun = psmrts_vector3d(9.0, 9.0, 9.0);

    PSMRTS_Vector3d obs1 = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d lkdr1 = psmrts_negate(&obs1);

    PSMRTS_PhotometricRayTrace *p_ray1 = psmrts_create_photometric_ray(&obs1, &lkdr1, &sun);

    PSMRTS_Vector3d obs2 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d lkdr2 = psmrts_negate(&obs2);

    PSMRTS_PhotometricRayTrace *p_ray2 = psmrts_create_photometric_ray(&obs2, &lkdr2, &sun);

    int first_index = psmrts_photometric_trace_array_add_trace(p_array, p_ray1);
    assert_int_equal(first_index, 0); // 0-base array

    size = psmrts_photometric_trace_array_size(p_array);
    assert_int_equal(size, 1);

    int second_index = psmrts_photometric_trace_array_add_trace(p_array, p_ray2);
    assert_int_equal(second_index, 1);

    size = psmrts_photometric_trace_array_size(p_array);
    assert_int_equal(size, 2);

    const PSMRTS_PhotometricRayTrace *target = psmrts_photometric_trace_array_get_trace(p_array, 1);
    const PSMRTS_RayTrace *target_ray = psmrts_photometric_observer_trace(target);
    PSMRTS_Vector3d target_obs = psmrts_ray_observer(target_ray);
    assert_double_equal(target_obs.x, obs2.x, tolerance);
    assert_double_equal(target_obs.y, obs2.y, tolerance);
    assert_double_equal(target_obs.z, obs2.z, tolerance);

    psmrts_free_photometric_trace_array( p_array );
    psmrts_free_photometric_ray( p_ray1 );
    psmrts_free_photometric_ray( p_ray2 );
}

// --- Conversion Fuctions ---
static void test_psmrts_conversions(void **state) {
    (void)state;

    // 50 degrees | 20 degrees | 100 radius
    PSMRTS_Vector3d lonlat = psmrts_vector3d(50.0, 20.0, 100); 

    PSMRTS_Vector3d xyz_result = psmrts_lonlatrad_to_xyz_d(&lonlat);
    assert_double_equal(xyz_result.x, 60.402277355505, tolerance);
    assert_double_equal(xyz_result.y, 71.984631039295, tolerance);
    assert_double_equal(xyz_result.z, 34.202014332567, tolerance);

    PSMRTS_Vector3d reverse = psmrts_xyz_to_lonlatrad_d(&xyz_result);
    assert_double_equal(reverse.x, lonlat.x, tolerance);
    assert_double_equal(reverse.y, lonlat.y, tolerance);
    assert_double_equal(reverse.z, lonlat.z, tolerance);

    double degree = 30;
    double deg2rad = psmrts_degrees_to_radians(degree);
    assert_double_equal(deg2rad, 0.523598775598, tolerance);

    double rad2deg = psmrts_radians_to_degrees(deg2rad);
    assert_double_equal(rad2deg, degree, tolerance);

    PSMRTS_Vector3d deg_vec = psmrts_vector3d(30, 75, 10.0);

    // Note: only converts first two values, 3rd is radius
    PSMRTS_Vector3d rad_vec = psmrts_vector3d_to_radians(&deg_vec);
    assert_double_equal(rad_vec.x, 0.523598775598, tolerance);
    assert_double_equal(rad_vec.y, 1.308996938996, tolerance);
    assert_double_equal(rad_vec.z, 10.0, tolerance);

    PSMRTS_Vector3d rev_vec = psmrts_vector3d_to_degrees(&rad_vec);
    assert_double_equal(rev_vec.x, deg_vec.x, tolerance);
    assert_double_equal(rev_vec.y, deg_vec.y, tolerance);
    assert_double_equal(rev_vec.z, deg_vec.z, tolerance);
}

// --- Tracer Functions --- 
static void test_psmrts_tracers(void **state) {
    (void)state;

    PSMRTS_Tracer *sphere = psmrts_create_sphere(1.0, "sphere");
    PSMRTS_BOOL sphere_valid = psmrts_tracer_valid( sphere );
    assert_int_equal(sphere_valid, 1);

    PSMRTS_Tracer *spheroid = psmrts_create_spheroid( 1.0, 2.0, "spheroid" );
    PSMRTS_BOOL spheroid_valid = psmrts_tracer_valid( spheroid );
    assert_int_equal(spheroid_valid, 1);

    PSMRTS_Tracer *ellipsoid = psmrts_create_ellipsoid(1.0, 2.0, 3.0, "ellipsoid");
    PSMRTS_BOOL ellipsoid_valid = psmrts_tracer_valid( ellipsoid );
    assert_int_equal(ellipsoid_valid, 1);

    PSMRTS_Vector3d e_vector = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Tracer *ellipsoid_v = psmrts_create_ellipsoid_v(&e_vector, "ellipsoid_v");
    PSMRTS_BOOL ellipsoid_v_valid = psmrts_tracer_valid( ellipsoid_v );
    assert_int_equal(ellipsoid_v_valid, 1);
    
    PSMRTS_Tracer *bullet = psmrts_create_bullet("../../../shapes/obj/data/bennu_20facets.obj");
    PSMRTS_BOOL bullet_valid = psmrts_tracer_valid( bullet );
    assert_int_equal(bullet_valid, 1);

    PSMRTS_Tracer *naifdsk = psmrts_create_naifdsk("../../../shapes/dsk/data/bennu_20facets.bds");
    PSMRTS_BOOL dsk_valid = psmrts_tracer_valid( naifdsk );
    assert_int_equal(dsk_valid, 1);
    
    psmrts_free_tracer( sphere );
    psmrts_free_tracer( spheroid );
    psmrts_free_tracer( ellipsoid );
    psmrts_free_tracer( ellipsoid_v );
    psmrts_free_tracer( bullet );
    psmrts_free_tracer( naifdsk );
}

int main(void) {
    cmocka_set_message_output(CM_OUTPUT_STDOUT);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_psmrts_version_info),
        cmocka_unit_test(test_psmrts_vector3d_init),
        cmocka_unit_test(test_psmrts_negate),
        cmocka_unit_test(test_psmrts_subtract),
        cmocka_unit_test(test_psmrts_add),
        cmocka_unit_test(test_psmrts_scale),
        cmocka_unit_test(test_psmrts_length),
        cmocka_unit_test(test_psmrts_ray),
        cmocka_unit_test(test_psmrts_raytrace),
        cmocka_unit_test(test_psmrts_trace_array),
        cmocka_unit_test(test_psmrts_photometric_trace),
        cmocka_unit_test(test_psmrts_photometric_array),
        cmocka_unit_test(test_psmrts_conversions),
        cmocka_unit_test(test_psmrts_tracers),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
