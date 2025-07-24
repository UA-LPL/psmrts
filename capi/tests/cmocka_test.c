#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "psmrts_c.h" 

static void test_psmrts_vector3d_init(void **state) {
    (void)state;

    PSMRTS_Vector3d v = psmrts_vector3d(1.0, 2.0, 3.0);

    assert_float_equal(v.x, 1.0, 1e-6);
    assert_float_equal(v.y, 2.0, 1e-6);
    assert_float_equal(v.z, 3.0, 1e-6);
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

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_psmrts_vector3d_init),
        cmocka_unit_test(test_psmrts_add),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
