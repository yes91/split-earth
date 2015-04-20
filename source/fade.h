#ifndef _fade_h_
#define _fade_h_

#include <gba_systemcalls.h>

static inline void fade_in(FIXED t, u32 mode, u32 bgs)
{
	REG_BLDCNT = mode << 6 | bgs;
	FIXED alpha = 0x100;
	REG_BLDY = alpha >> 4;

	FIXED step = fx_div(float_to_fx(1.f/60.f), t);

	while(alpha > 0x0)
	{
		alpha -= step;
		REG_BLDY = alpha >> 4;
		VBlankIntrWait();
	}
}

static inline void fade_out(FIXED t, u32 mode, u32 bgs)
{
	REG_BLDCNT = mode << 6 | bgs;
	FIXED alpha = 0x0;
	REG_BLDY = alpha >> 4;

	FIXED step = fx_div(float_to_fx(1.f/60.f), t);

	while(alpha < 0x100)
	{
		alpha += step;
		REG_BLDY = alpha >> 4;
		VBlankIntrWait();
	}
}

#endif
