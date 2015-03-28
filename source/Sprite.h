#ifndef _Sprite_h_
#define _Sprite_h_

#include "Vector2.h"

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
	u32 width;
	u32 height;
	u32 count;
	Animation* clips;
} AnimContainer;

typedef struct Sprite
{
	struct SPR_BASE* base;
	int pal, tile;
	int anim;
	int frame;
	int timer;
	AnimContainer anims;
} Sprite;

void Sprite_construct(Sprite* self, FIXED x, FIXED y, u16 shape, u16 size, int pal, int tile);

void Sprite_draw(Sprite* self, FIXED offset_x, FIXED offset_y);

void Sprite_play(Sprite* self, u32 anim);

void Sprite_destroy(Sprite* self);

void AnimContainer_decode(AnimContainer* dst, const u8* src);

#endif
