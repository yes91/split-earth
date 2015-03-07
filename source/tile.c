#include "tile.h"

void tile_copy(TILE* dst, const TILE* src, u32 count)
{
	while(count--)
		*dst++ = *src++;
}

void tile8_copy(TILE8* dst, const TILE8* src, u32 count)
{
	while(count--)
		*dst++ = *src++;
}
