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
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
