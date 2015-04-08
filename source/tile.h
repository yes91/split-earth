#ifndef _tile_h_
#define _tile_h_

#include <gba_types.h>

// tile 8x8@4bpp: 32bytes; 8 ints
typedef struct TILE { u32 data[8]; } TILE, TILE4;
// d-tile: double-sized tile (8bpp)
typedef struct TILE8 { u32 data[16]; } TILE8;
// tile block: 32x16 tiles, 16x16 d-tiles
typedef TILE  CHARBLOCK[512];
typedef TILE8 CHARBLOCK8[256];

#define tile_mem  ( (CHARBLOCK*)0x06000000)
#define tile8_mem ((CHARBLOCK8*)0x06000000)

void tile_copy(TILE* dst, const TILE* src, u32 count);

void tile8_copy(TILE8* dst, const TILE8* src, u32 count);

#endif
