#include "Sprite.h"

#include "oam_manager.h"
#include "tile.h"
#include <stdlib.h>
#include <string.h>

#include "debug.h"

void Sprite_construct(Sprite* self, FIXED x, FIXED y, u16 shape, u16 size, int pal, int tile)
{
	self->pal = pal;
	self->tile = tile;
	self->anim = 0;
	self->frame = 0;
	self->timer = 0;
	self->anims.count = 0;

	self->base = spr_alloc();
	self->base->pos.x = x;
	self->base->pos.y = y;
	self->base->oam.attr0 = shape | OBJ_Y(y);
	self->base->oam.attr1 = size | OBJ_X(x);
	self->base->oam.attr2 = OBJ_PALETTE(pal) | OBJ_CHAR(tile);
}

void Sprite_play(Sprite* self, u32 anim)
{
	if(anim == self->anim) return;

	self->anim = anim;
	self->frame = 0;
	self->timer = 0;
}

void Sprite_draw(Sprite* self, FIXED offset_x, FIXED offset_y)
{
	OBJATTR* obj = &self->base->oam;
	obj->attr0 = (obj->attr0 &~ 0x00FF) | OBJ_Y(fx_to_int(self->base->pos.y - offset_y));
	obj->attr1 = (obj->attr1 &~ 0x01FF) | OBJ_X(fx_to_int(self->base->pos.x - offset_x));

	if(self->anims.count != 0 && self->timer == 0)
	{
		Frame current = self->anims.clips[self->anim].frames[self->frame];
		u32 offset = current.index * self->anims.width * self->anims.height;
		
		obj->attr2 = OBJ_PALETTE(self->pal) | OBJ_CHAR(self->tile + offset);
		self->frame = (self->frame + 1) % self->anims.clips[self->anim].count;
		
		self->timer = current.duration;
	}
	else
	{
		obj->attr2 = (obj->attr2 & 0x03FF) | OBJ_PALETTE(self->pal);
	}

	self->timer--;

	if(self->timer < 0) self->timer = 0;
}

void Sprite_destroy(Sprite* self)
{
	spr_free(self->base);

	int i;
	for(i = 0; i < self->anims.count; i++)
	{
		free(self->anims.clips[i].frames);
	}

	free(self->anims.clips);
}

#define READ(src, type) *(type*)src; src += sizeof(type)

void AnimContainer_decode(AnimContainer* dst, const u8* src)
{
	u32 width = READ(src, u32);
	u32 height = READ(src, u32);
	u32 anim_count = READ(src, u32);
	
	dst->width = width;
	dst->height = height;
	dst->count = anim_count;

	dst->clips = malloc(sizeof(Animation) * anim_count);
	int i;
	for(i = 0; i < anim_count; i++)
	{
		Animation anim;
		anim.count = READ(src, u32);
		u32 size = sizeof(Frame) * anim.count;
		anim.frames = malloc(size);

		memcpy(anim.frames, src, size);
		src += size;

		dst->clips[i] = anim;
	}

}
