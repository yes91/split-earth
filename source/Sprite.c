#include "Sprite.h"

#include "oam_manager.h"
#include "tile.h"
#include "filesys.h"

#include "debug.h"

static int tile_count(u16 size, u16 shape)
{
	return 1 << ((size >> 13) - (shape >> 14 > 0));
}

// This is in log base 2
static int log_tile_width(u16 size, u16 shape)
{
	return ((size >> 14) >> (shape >> 14 == 2));
}

void Sprite_construct(Sprite* self, Vector2 pos, u16 shape, u16 size, int pal, int tile)
{
	self->pal = pal;
	self->tile = tile;
	self->tiles = tile_count(size, shape);
	self->anim = 0;
	self->frame = 0;
	self->timer = 0;
	self->anims.count = 0;

	self->base = spr_alloc();

	u32 lwidth = log_tile_width(size, shape);	// This is in log base 2
	u32 hwidth = 8 << (lwidth - 1);
	u32 hheight = self->tiles * 8 >> (lwidth + 1);
	Vector2 halves = Vector2_create(int_to_fx(hwidth), int_to_fx(hheight));
	self->base->pos = pos;
	self->base->mid = halves;
	self->base->half = halves;
	self->base->oam.attr0 = shape | OBJ_Y(fx_to_int(pos.y));
	self->base->oam.attr1 = size | OBJ_X(fx_to_int(pos.x));
	self->base->oam.attr2 = OBJ_PALETTE(pal) | OBJ_CHAR(tile);
}

void Sprite_toggle(Sprite* self)
{
	self->base->oam.attr0 ^= ATTR0_DISABLED;
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
		u32 offset = current.index * self->tiles;
		
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

void Sprite_decode(Sprite* self, int tile, const u8* src)
{
	u16 size;
	src += read_data(&size, src, sizeof size);

	u16 shape;
	src += read_data(&shape, src, sizeof shape);

	bool bounds_set;
	src += read_data(&bounds_set, src, sizeof bounds_set);

	Vector2 bounds[2];
	if(bounds_set)
	{
		src += read_data(&bounds, src, sizeof bounds);
	}

	u8 pal;
	src += read_data(&pal, src, sizeof pal);

	Sprite_construct(self, Vector2_create(0, 0), shape, size, pal, tile);

	if(bounds_set)
	{
		memcpy(&self->base->mid, bounds, sizeof bounds);
	}
	
	u32 anim_count;
	src += read_data(&anim_count, src, sizeof anim_count);

	AnimContainer* dst = &self->anims;

	dst->count = anim_count;

	dst->clips = malloc(sizeof(Animation) * anim_count);
	int i;
	for(i = 0; i < anim_count; i++)
	{
		Animation anim;
		src += read_data(&anim.count, src, sizeof anim.count);

		u32 size = sizeof(Frame) * anim.count;
		anim.frames = malloc(size);

		src += read_data(anim.frames, src, size);

		dst->clips[i] = anim;
	}
}

Vector2* Sprite_pos(Sprite* self)
{
	return &self->base->pos;
}

Vector2* Sprite_mid(Sprite* self)
{
	return &self->base->mid;
}

Vector2* Sprite_half(Sprite* self)
{
	return &self->base->half;
}
