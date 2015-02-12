#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gba_types.h>

#define MODE5_WIDTH 160
#define MODE5_HEIGHT 128

#define MODE3_WIDTH 240
#define MODE3_HEIGHT 160

#define MODE4_WIDTH 120
#define MODE4_HEIGHT 160

struct Mode4_ {
	u32 width;
	u32 height;
	u16* paletteMem;
	u16* frontBuffer;
	u16* backBuffer;
	u16* currentBuffer;
};

typedef struct Mode4_ Mode4;

Mode4 Mode4_new();

void flip(Mode4* g);

void clearScreen(Mode4* g, u8 clr);

#endif
