#ifndef _util_h_
#define _util_h_

static inline int clamp(int min, int max, int val)
{
	if(val < min)
		return min;
	else if(val > max)
		return max;
	else
		return val;
}

#endif
