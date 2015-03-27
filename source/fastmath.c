#include "fastmath.h"

extern short lut_bin[];

short* COS = &lut_bin[0];

short* SIN = &lut_bin[360];

short fx_cos(int x)
{
	return COS[x];
}

short fx_sin(int x)
{
	return SIN[x];
}
