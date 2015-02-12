#include "Graphics.h"

#include <gba_video.h>

Mode4 Mode4_new()
{
	Mode4 mode = {  MODE4_WIDTH, 
		 	MODE4_HEIGHT, 
		 	(u16*)0x5000000,
		 	(u16*)0x06000000,
		 	(u16*)0x0600A000,
		 	NULL };
	return mode;
}

void flip(Mode4* g)
{
	REG_DISPCNT ^= BACKBUFFER;
	if (g->currentBuffer == g->frontBuffer)
	{
		g->currentBuffer = g->backBuffer;
	}
	else
	{
		g->currentBuffer = g->frontBuffer;
	}
}

void clearScreen(Mode4* g, u8 clr)
{

	int i;

	for (i = 0; i < g->width * g->height; i++)
		g->currentBuffer[i] = clr|(clr<<8);

}
