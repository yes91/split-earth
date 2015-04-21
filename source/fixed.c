#include "fixed.h"
#include <gba_base.h>

#define FIXED_INVLN2 369 //1 / ln(2) = 1.442695 as an 8 bit fraction
#define FIXED_LN2 177 // ln(2) = 0.693147 as an 8 bit fraction

FIXED fx_log2(FIXED x)
{
	FIXED b = FIX_SCALE / 2;
	FIXED y = 0;

	//Normalization to 1 < x <= 2 
	while(x < FIX_SCALE)
	{
		x <<= 1;
		y -= FIX_SCALE;
	}

	while(x >= 2 * FIX_SCALE)
	{
		x >>= 1;
		y += FIX_SCALE;
	}

	//Now, with normalization complete
	u32 i;
	for(i = 0; i < FIX_SHIFT; i++)
	{
		x = fx_mul(x, x);
		if(x >= 2 * FIX_SCALE)
		{
			x >>= 1;
			y += b;
		}
		b >>= 1;
	}

	return y;
}

extern FIXED fx_exp2_lut_bin[FIX_SCALE];

FIXED fx_exp2(FIXED x)
{
	FIXED b, f;
	b = 1 << (FIX_SHIFT + (x >> FIX_SHIFT)); //calculate base
	f = fx_exp2_lut_bin[x & FIX_MASK]; //lookup fractional part
	return fx_mul(b, f);
}

FIXED fx_log(FIXED x)
{
	return fx_mul(fx_log2(x), FIXED_LN2);
}

FIXED fx_exp(FIXED x)
{
	return fx_exp2(fx_mul(x, FIXED_INVLN2));
}

FIXED fx_sqrt(FIXED n)
{
	return fx_exp2(fx_mul(FIX_SCALE/2, fx_log2(n)));
}

FIXED fx_inv_sqrt(FIXED n)
{
	return fx_exp2(fx_mul(-(FIX_SCALE/2), fx_log2(n)));
}