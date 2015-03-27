#include "minunit.h"

#include "../source/fixed.h"
#include <math.h>

typedef FIXED(*fx_func)(FIXED);

typedef double(*float_func)(double);

char* test_error_tolerance(fx_func subject, float_func standard, float value)
{
	FIXED expected = float_to_fx(standard(value));
	FIXED actual = subject(float_to_fx(value));

	debug("expected == %d, actual == %d\n", expected, actual);

	float real = standard(value);
	float approx = fx_to_float(subject(float_to_fx(value)));
	float error = fabs(approx - real) / fabs(real) * 100.f;

	debug("exact value == %f, approx == %f\n", real, approx);
	debug("percent error == %f\n", error);

	mu_assert(error < 0.5f, "Failed error tolerance");
	
    return NULL;
}

char* test_sqrt_error_tolerance()
{
    return test_error_tolerance(fx_sqrt, sqrt, 0.5f);
}

char* test_exp_error_tolerance()
{
    return test_error_tolerance(fx_exp, exp, 0.5f);
}

char* test_log_error_tolerance()
{
    return test_error_tolerance(fx_log, log, 0.5f);
}

char* test_exp2_error_tolerance()
{
    return test_error_tolerance(fx_exp2, exp2, 4.345f);
}

char* test_log2_error_tolerance()
{
    return test_error_tolerance(fx_log2, log2, 4.345f);
}

char* all_tests()
{
    mu_suite_start();

    mu_run_test(test_sqrt_error_tolerance);
 	mu_run_test(test_exp_error_tolerance);
	mu_run_test(test_log_error_tolerance);
	mu_run_test(test_exp2_error_tolerance);
	mu_run_test(test_log2_error_tolerance);	

    return NULL;
}

RUN_TESTS(all_tests)


