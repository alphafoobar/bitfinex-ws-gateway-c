#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "bitfinex/bitfinex-depth.h"

static void
test_higher() {
    assert_true(fp_equals(9119.4, 9119.36, 0.5));
    assert_true(fp_equals(9119.4, 9119.3999999999, 0.01));
}

static void
test_lower() {
    assert_true(fp_equals(9119.3, 9119.34, 0.5));
    assert_true(fp_equals(9119.3, 9119.309, 0.01));
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_higher),
            cmocka_unit_test(test_lower),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
