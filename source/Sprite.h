#ifndef _Sprite_h_
#define _Sprite_h_

#include <gba_sprites.h>

typedef struct Sprite
{
	int x, y;
	OBJATTR* oam;
	int gfxID; 
} Sprite;

Sprite Sprite_new(int x, int y, u16 shape, u16 size, int gfxID);

void Sprite_draw(Sprite* self);

void Sprite_destroy(Sprite* self);

#endif
