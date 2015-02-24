#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gba_types.h>

#define MODE5_WIDTH 160
#define MODE5_HEIGHT 128

#define MODE3_WIDTH 240
#define MODE3_HEIGHT 160

#define MODE4_WIDTH 120
#define MODE4_HEIGHT 160

struct Graphics_ {
	u32 width;
	u32 height;
	u32 mode;
	u16* paletteBuffer;
	void* frontBuffer;
	void* backBuffer;
	void* currentBuffer;
};

typedef struct Graphics_ Graphics;

Graphics Graphics_new(int options);

void Graphics_setMode(Graphics* g, int options);

void flip(Graphics* g);

void clearScreen(Graphics* g, u32 clr);

#endif
