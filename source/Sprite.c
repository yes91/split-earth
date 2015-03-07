#include "Sprite.h"

#include "oam_manager.h"
#include "tile.h"
#include <stdlib.h>

#include "debug.h"

void Sprite_construct(Sprite* self, int x, int y, u16 shape, u16 size, int pal, int tile)
{
	self->x = x;
	self->y = y;
	self->pal = pal;
	self->tile = tile;
	self->anim = 0;
	self->frame = 0;
	self->timer = 0;
	self->anims.count = 0;

	self->oam = obj_alloc();
	self->oam->attr0 = shape | OBJ_Y(y);
	self->oam->attr1 = size | OBJ_X(x);
	self->oam->attr2 = OBJ_PALETTE(pal) | OBJ_CHAR(tile);
}

void Sprite_play(Sprite* self, u32 anim)
{
	if(anim == self->anim) return;

	self->anim = anim;
	self->timer = 0;
}

void Sprite_draw(Sprite* self)
{
	OBJATTR* obj = self->oam;
	obj->attr0 = (obj->attr0 &~ 0x00FF) | OBJ_Y(self->y);
	obj->attr1 = (obj->attr1 &~ 0x01FF) | OBJ_X(self->x);

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
	obj_free(self->oam);
	
	int i;
	for(i = 0; i < self->anims.count; i++)
	{
		free(self->anims.clips[i].frames);
	}

	free(self->anims.clips);
}

void AnimContainer_decode(AnimContainer* dst, const u8* src)
{
	u32 width = *((u32*) src);
	src += sizeof(u32);

	u32 height = *((u32*) src);
	src += sizeof(u32);

	u32 anim_count = *((u32*) src);
	src += sizeof(u32);
	
	dst->width = width;
	dst->height = height;
	dst->count = anim_count;

	dst->clips = malloc(sizeof(Animation) * anim_count);
	int i;
	for(i = 0; i < anim_count; i++)
	{
		Animation anim;
		anim.count = *(u32*)src;
		src += sizeof(u32);
		anim.frames = malloc(sizeof(Frame) * anim.count);

		Frame* from = (Frame*)src;
		Frame* to = anim.frames;

		int count = anim.count;
		while(count--)
			*to++ = *from++;

		src += sizeof(Frame) * anim.count;

		dst->clips[i] = anim;
	}

}
