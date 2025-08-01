#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "psmrts_c.h" 

// --- PSMRTS information Functions ---
static void test_psmrts_version_info(void **state) {
    (void)state;

    const char* version = psmrts_version();
    const char* info = psmrts_info();

    assert_string_equal(version, "0.2.0");
    assert_string_equal(info, "psmrts-0.2.0");
}

// --- PSMRTS Vector3d Functions ---
static void test_psmrts_vector3d_init(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 2.0, 3.0);

    assert_float_equal(v.x, 1.0, 1e-6);
    assert_float_equal(v.y, 2.0, 1e-6);
    assert_float_equal(v.z, 3.0, 1e-6);
}

static void test_psmrts_negate(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(2.0, 3.0, 4.0);
    PSMRTS_Vector3d v_neg = psmrts_negate( &v );

    assert_float_equal(v_neg.x, -2.0, 1e-6);
    assert_float_equal(v_neg.y, -3.0, 1e-6);
    assert_float_equal(v_neg.z, -4.0, 1e-6);

    PSMRTS_Vector3d v2 = psmrts_vector3d(2.0, -3.0, 4.0);
    PSMRTS_Vector3d v2_neg = psmrts_negate( &v2 );

    assert_float_equal(v2_neg.x, -2.0, 1e-6);
    assert_float_equal(v2_neg.y, 3.0, 1e-6);
    assert_float_equal(v2_neg.z, -4.0, 1e-6);
}

static void test_psmrts_subtract(void **state) {
    (void)state;

    PSMRTS_Vector3d v1 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d v2 = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d result = psmrts_subtract(&v1, &v2);

    assert_float_equal(result.x, 3.0, 1e-6);
    assert_float_equal(result.y, 3.0, 1e-6);
    assert_float_equal(result.z, 3.0, 1e-6);

    PSMRTS_Vector3d v3 = psmrts_vector3d(0.0, 10.0, -4.0);
    PSMRTS_Vector3d v4 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d result2 = psmrts_subtract(&v3, &v4);

    assert_float_equal(result2.x, -4.0, 1e-6);
    assert_float_equal(result2.y, 5.0, 1e-6);
    assert_float_equal(result2.z, -10.0, 1e-6);
}

static void test_psmrts_add(void **state) {
    (void)state;

    PSMRTS_Vector3d v1 = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d v2 = psmrts_vector3d(4.0, 5.0, 6.0);
    PSMRTS_Vector3d result = psmrts_add(&v1, &v2);

    assert_float_equal(result.x, 5.0, 1e-6);
    assert_float_equal(result.y, 7.0, 1e-6);
    assert_float_equal(result.z, 9.0, 1e-6);
}

static void test_psmrts_scale(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 2.0, 3.0);
    PSMRTS_Vector3d result = psmrts_scale(&v, 2);

    assert_float_equal(result.x, 2.0, 1e-6);
    assert_float_equal(result.y, 4.0, 1e-6);
    assert_float_equal(result.z, 6.0, 1e-6);
}

static void test_psmrts_length(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 4.0, 8.0);
    double result = psmrts_length(&v);

    assert_float_equal(result, 9.0, 1e-6);
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

    assert_float_equal(obs_result.x, 2.0, 1e-6);
    assert_float_equal(obs_result.y, 4.0, 1e-6);
    assert_float_equal(obs_result.z, 6.0, 1e-6);

    assert_float_equal(lkdr_result.x, -1.0, 1e-6);
    assert_float_equal(lkdr_result.y, 0.0, 1e-6);
    assert_float_equal(lkdr_result.z, 12.0, 1e-6);

    PSMRTS_BOOL nohit = psmrts_ray_has_hit(ray);

    assert_int_equal(nohit, 0);

    /*
    PSMRTS_Vector3d raypt = psmrts_ray_raypt(ray);
    assert_int_equal(raypt.x, 0.0);
    assert_int_equal(raypt.y, 0.0);
    assert_int_equal(raypt.z, 0.0);
    // checked just to see, numbers result in hexadec #s
    */

    PSMRTS_Vector3d new_obs = psmrts_vector3d(3.0, -5.0, 7.0);
    PSMRTS_Vector3d new_lkdr = psmrts_vector3d(100.0, 20.2, 30.0);

    psmrts_ray_set_observation(&new_obs, &new_lkdr, ray);

    obs_result = psmrts_ray_observer(ray);
    lkdr_result = psmrts_ray_lookdir(ray);

    assert_float_equal(obs_result.x, 3.0, 1e-6);
    assert_float_equal(obs_result.y, -5.0, 1e-6);
    assert_float_equal(obs_result.z, 7.0, 1e-6);

    assert_float_equal(lkdr_result.x, 100.0, 1e-6);
    assert_float_equal(lkdr_result.y, 20.2, 1e-6);
    assert_float_equal(lkdr_result.z, 30.0, 1e-6);
}

static void test_psmrts_raytrace(void **state) {
    (void)state;
    const char *name = "test";
    PSMRTS_Tracer *ellipse = psmrts_create_sphere(1.0, name);

    PSMRTS_Vector3d obs = psmrts_vector3d(0.0, 0.0, 3.0);
    PSMRTS_Vector3d lkdr = psmrts_vector3d(0.0, 0.0, -1.0);
    PSMRTS_RayTrace *ray = psmrts_create_ray(&obs, &lkdr);

    PSMRTS_RayTrace *raytrace = psmrts_ray_trace(ray, ellipse);

    PSMRTS_BOOL hashit = psmrts_ray_has_hit(raytrace);
    assert_int_equal(hashit, 1);

    PSMRTS_Vector3d xyz = psmrts_ray_xyz(raytrace);
    assert_float_equal(xyz.x, 0.0, 1e-6);
    assert_float_equal(xyz.y, 0.0, 1e-6);
    assert_float_equal(xyz.z, 1.0, 1e-6);

    PSMRTS_Vector3d raypt = psmrts_ray_raypt(raytrace);
    assert_float_equal(raypt.x, 0.0, 1e-6);
    assert_float_equal(raypt.y, 0.0, 1e-6);
    assert_float_equal(raypt.z, -2.0, 1e-6);

    PSMRTS_Vector3d normal = psmrts_ray_normal(raytrace);
    assert_float_equal(normal.x, 0.0, 1e-6);
    assert_float_equal(normal.y, 0.0, 1e-6);
    assert_float_equal(normal.z, 1.0, 1e-6);

    double intercept = psmrts_ray_intercept_radius(raytrace);
    assert_float_equal(intercept, 1.0, 1e-6);

    double slant = psmrts_ray_intercept_slant_distance(raytrace);
    assert_float_equal(slant, 2.0, 1e-6);

    // Second Ray Trace
    PSMRTS_Vector3d obs2 = psmrts_vector3d(0.0, 2.0, 4.0);
    PSMRTS_Vector3d lkdr2 = psmrts_vector3d(0.0, -2.0, -2.0);
    PSMRTS_RayTrace *ray2 = psmrts_create_ray(&obs, &lkdr);

    PSMRTS_RayTrace *raytrace2 = psmrts_ray_trace(ray2, ellipse);
    PSMRTS_BOOL hashit2 = psmrts_ray_has_hit(raytrace2);
    assert_int_equal(hashit2, 1); 

    double dist = psmrts_ray2ray_distance(raytrace, raytrace2);
    assert_float_equal(dist, 0.0, 1.e-6); // Should this be 0?

    PSMRTS_Vector3d v1 = psmrts_vector3d(1.0, 0.0, 0.0);
    PSMRTS_Vector3d v2 = psmrts_vector3d(1.0, 1.0, 0.0); 
    double sep_angle = psmrts_separation_angle_radians(&v1, &v2);
    assert_float_equal(sep_angle, 0.785398, 1e-6);

    PSMRTS_BOOL near = psmrts_isNear(raytrace, raytrace2, 5.0);
    assert_int_equal(near, 1);

    double incidence = psmrts_incidence(raytrace, raytrace2);
    assert_float_equal(incidence, 0.0, 1e-6);

    double emission = psmrts_emission(raytrace);
    assert_float_equal(emission, 0.0, 1e-6);

    double phase = psmrts_phase(raytrace, raytrace2);
    assert_float_equal(phase, 0.0, 1e-6);
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

    PSMRTS_RayTrace *raytrace1 = psmrts_ray_trace(ray, ellipse);

    psmrts_trace_array_add_trace(t_array, raytrace1);
    size = psmrts_trace_array_size(t_array);
    assert_int_equal(size, 1);

    PSMRTS_Vector3d obs2 = psmrts_vector3d(0.0, 3.0, 2.0);
    PSMRTS_Vector3d lkdr2 = psmrts_vector3d(0.0, -2.0, -1.0);
    PSMRTS_RayTrace *ray2 = psmrts_create_ray(&obs2, &lkdr2);

    PSMRTS_RayTrace *raytrace2 = psmrts_ray_trace(ray2, ellipse);

    psmrts_trace_array_add_trace(t_array, raytrace2);
    size = psmrts_trace_array_size(t_array);
    assert_int_equal(size, 2);

    const PSMRTS_RayTrace *first = psmrts_trace_array_get_trace(t_array, 0);
    PSMRTS_Vector3d first_obs = psmrts_ray_observer(first);
    assert_float_equal(first_obs.x, 0.0, 1e-6);
    assert_float_equal(first_obs.y, 0.0, 1e-6);
    assert_float_equal(first_obs.z, 3.0, 1e-6);

    const PSMRTS_RayTrace *second = psmrts_trace_array_get_trace(t_array, 1);
    PSMRTS_Vector3d sec_obs = psmrts_ray_observer(second);
    assert_float_equal(sec_obs.x, 0.0, 1e-6);
    assert_float_equal(sec_obs.y, 3.0, 1e-6);
    assert_float_equal(sec_obs.z, 2.0, 1e-6);
}

// --- PSMRTS Photometric Trace/Array Functions ---
static void test_psmrts_photometric_trace(void **state) {
    (void)state;

    const char *name = "test";
    PSMRTS_Tracer *ellipse = psmrts_create_sphere(1.0, name);

    PSMRTS_Vector3d obs = psmrts_vector3d(45.0, 45.0, 1.0);
    obs = psmrts_lonlatrad_to_xyz_d(&obs);
    obs = psmrts_scale(&obs, 10.0);

    PSMRTS_Vector3d lkdr = psmrts_negate(&obs);

    PSMRTS_RayTrace *observer_ray = psmrts_ray_trace_v(&obs, &lkdr, ellipse);
    PSMRTS_BOOL obs_hit = psmrts_ray_has_hit(observer_ray);
    assert_int_equal(obs_hit, 1);

    PSMRTS_Vector3d sun_pos = psmrts_vector3d(20.0, 20.0, 1.0);
    sun_pos = psmrts_lonlatrad_to_xyz_d(&sun_pos);
    sun_pos = psmrts_scale(&sun_pos, 50.0);

    PSMRTS_PhotometricRayTrace *p_ray = psmrts_create_photometric_ray(&obs, &lkdr, &sun_pos);

    /** Need photometric specific ray trace to test further
    double incidence = psmrts_photometric_incidence(p_ray);
    assert_float_equal(incidence, 0.0, 1e-6);

    double emission = psmrts_photometric_emission(p_ray);
    assert_float_equal(emission, 0.0, 1e-6);

    double phase = psmrts_photometric_phase(p_ray);
    assert_float_equal(phase, 0.0, 1e-6);
    */

    const PSMRTS_RayTrace *obs_ray = psmrts_photometric_observer_trace(p_ray);
    PSMRTS_Vector3d obs_result = psmrts_ray_observer(obs_ray);
    assert_float_equal(obs_result.x, obs.x, 1e-6);
    assert_float_equal(obs_result.y, obs.y, 1e-6);
    assert_float_equal(obs_result.z, obs.z, 1e-6);

    // For this, would the sun be the observer? How can we retrieve the
    // sunpos from this structure..?
    const PSMRTS_RayTrace *sun_ray = psmrts_photometric_sun_trace(p_ray);
    //PSMRTS_Vector3d sun_result = psmrts_ray_observer(obs_ray);
    //assert_float_equal(sun_result.x, sun_pos.x, 1e-6);
    //assert_float_equal(sun_result.y, sun_pos.y, 1e-6);
    //assert_float_equal(sun_result.z, sun_pos.z, 1e-6);

    PSMRTS_Vector3d new_obs = psmrts_vector3d(2.0, 4.0, 6.0);
    PSMRTS_Vector3d new_lkdr = psmrts_negate(&new_obs);

    psmrts_photometric_ray_set_observation(&new_obs, &new_lkdr, &sun_pos, p_ray);
    const PSMRTS_RayTrace *new_obs_ray = psmrts_photometric_observer_trace(p_ray);
    PSMRTS_Vector3d new_obs_result = psmrts_ray_observer(new_obs_ray);
    assert_float_equal(new_obs_result.x, new_obs.x, 1e-6);
    assert_float_equal(new_obs_result.y, new_obs.y, 1e-6);
    assert_float_equal(new_obs_result.z, new_obs.z, 1e-6);
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

    // May need to consider obs, look dir, sun getters for photometric ray traces
    // Also, should I need to be casting const values here?
    /** 
    PSMRTS_PhotometricRayTrace *target = psmrts_photometric_trace_array_get_trace(p_array, 1);
    PSMRTS_RayTrace *target_ray = psmrts_photometric_observer_trace(target);
    PSMRTS_Vector3d target_obs = psmrts_ray_observer(target_ray);
    assert_float_equal(target_obs.x, obs2.x, 1e-6);
    assert_float_equal(target_obs.y, obs2.y, 1e-6);
    assert_float_equal(target_obs.z, obs2.z, 1e-6);
    */
}

int main(void) {
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
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
