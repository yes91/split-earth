#include "Graphics.h"

#include <gba_video.h>

void Graphics_setMode(Graphics* g, int options)
{
	SetMode( options );
	g->mode = options & 0x07; //grab bits 0 - 2, representing the graphics mode
	switch(g->mode)
	{
		case 0:
		case 1:
		case 2:
		
		case 3:
			g->width = MODE3_WIDTH;
			g->height = MODE3_HEIGHT;
			break;
		case 4:
			g->width = MODE4_WIDTH;
			g->height = MODE4_HEIGHT;
			break;
		case 5:
			g->width = MODE5_WIDTH;
			g->height = MODE5_HEIGHT;
	}
	
}

Graphics Graphics_new(int options)
{
	Graphics g = {  0,
		 	0,
			0,
		 	(u16*)0x5000000,
		 	(void*)0x06000000,
		 	(void*)0x0600A000,
		 	(void*)0x0600A000 };
	Graphics_setMode(&g, options);
	return g;
}

void flip(Graphics* g)
{
	REG_DISPCNT ^= BACKBUFFER;

	u32 v_buff = (u32)g->currentBuffer;
	g->currentBuffer = (void*)(v_buff ^ 0xA000);
}

void clearScreen(Graphics* g, u32 clr)
{
	int i;

	u32 color = clr|(clr<<8);

	u16* dst = g->currentBuffer;
	
	/*switch(g->mode)
	{
		case 5:
		case 3:
			color = clr|(clr<<16);
			break;
		case 4:
			color = clr|(clr<<8)|(clr<<16)|(clr<<24);
	}*/

	u32 size = g->width * g->height;

	for (i = 0; i < size; i++)
		dst[i] = color;
}
