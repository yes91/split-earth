#include "tile.h"

void tile_copy(TILE* dst, const TILE* src, u32 count)
{
	while(count--)
		*dst++ = *src++;
}
