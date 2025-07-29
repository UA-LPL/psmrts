#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "psmrts_c.h" 

static void test_psmrts_version_info(void **state) {
    (void)state;

    const char* version = psmrts_version();
    const char* info = psmrts_info();

    assert_string_equal(version, "0.2.0");
    assert_string_equal(info, "psmrts-0.2.0");
}


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

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_psmrts_version_info),
        cmocka_unit_test(test_psmrts_vector3d_init),
        cmocka_unit_test(test_psmrts_negate),
        cmocka_unit_test(test_psmrts_subtract),
        cmocka_unit_test(test_psmrts_add),
        cmocka_unit_test(test_psmrts_scale),
        cmocka_unit_test(test_psmrts_length),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
