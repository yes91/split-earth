#ifndef _Sprite_h_
#define _Sprite_h_

#include "Vector2.h"

struct GBFS_FILE;

struct SPR_BASE;

typedef struct Frame 
{ 
	u32 index; 
	u32 duration;
} Frame;

typedef struct Animation
{
	u32 count;
	Frame* frames;
} Animation;

typedef struct AnimContainer
{
	u32 count;
	Animation* clips;
} AnimContainer;

typedef struct Sprite
{
	struct SPR_BASE* base;
	int pal, tile;
	int tiles;
	int anim;
	int frame;
	int timer;
	AnimContainer anims;
} Sprite;

void Sprite_construct(
	Sprite* self,
	Vector2 pos,
	u16 shape,
	u16 size,
	int pal,
	int tile );

void Sprite_decode(Sprite* self, int tile, const u8* src);

void Sprite_draw(Sprite* self, FIXED offset_x, FIXED offset_y);

void Sprite_play(Sprite* self, u32 anim);

void Sprite_destroy(Sprite* self);

#endif
