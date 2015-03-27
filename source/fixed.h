#ifndef _fixed_h_
#define _fixed_h_

#include <gba_types.h>

typedef s32 FIXED;

#define FIX_SHIFT 8
#define FIX_SCALE (1<<FIX_SHIFT)
#define FIX_SCALEF ((float)FIX_SCALE)
#define FIX_MASK (FIX_SCALE-1)

#define FIXED_PI ((FIXED)(3.14159f * FIX_SCALEF))

static inline FIXED int_to_fx(int x)
{
	return x << FIX_SHIFT;
}

static inline FIXED float_to_fx(float x)
{
	return (FIXED) (x * FIX_SCALEF);
}

static inline u32 fx_to_uint(FIXED x)
{
	return x >> FIX_SHIFT;
}

static inline int fx_to_int(FIXED x)
{
	return x / FIX_SCALE;
}

static inline float fx_to_float(FIXED x)
{
	return x / FIX_SCALEF;
}

static inline int fx_round(FIXED x)
{
	return (x + FIX_SCALE/2) / FIX_SCALE;
}

static inline FIXED fx_mul(FIXED a, FIXED b)
{
	return (a * b) >> FIX_SHIFT; 
}

static inline FIXED fx_div(FIXED a, FIXED b)
{
	return (a * FIX_SCALE) / b;
}

FIXED fx_log2(FIXED x);

FIXED fx_exp2(FIXED x);

FIXED fx_log(FIXED x);

FIXED fx_exp(FIXED x);

FIXED fx_sqrt(FIXED n);

FIXED fx_inv_sqrt(FIXED n);

#endif
