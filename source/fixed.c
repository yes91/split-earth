#include "fixed.h"

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

FIXED fx_sqrt(FIXED n) 
{
    FIXED s = (n + FIX_SCALE * FIX_SCALE) >> 1;
    u32 i;
 
    for (i = 0; i < FIX_SHIFT; i++) {
        s = (s + fx_div(n, s)) >> 1;
    }
 
    return s;
}