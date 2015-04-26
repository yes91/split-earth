#include "Character.h"

#include "tile.h"
#include "oam_manager.h"
#include <stddef.h>
#include <gbfs.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include "spr_vram_manager.h"
#include "filesys.h"
#include "util.h"

#include "debug.h"

void Character_construct(
	Character* self,
	Vector2 pos,
	const GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* sprite,
	int palette
	)
{
	self->velocity = Vector2_create(0, 0);
	self->forward = Vector2_create(0, int_to_fx(-1));
	self->heading = NORTH;
	self->z = 0;

	u32 char_size = 0;
	const TILE* char_tiles = gbfs_get_obj(dat, image, &char_size);

	u32 char_pal_len = 0;
	const u16* char_pal = gbfs_get_obj(dat, pal, &char_pal_len);

	const u8* char_sprite = gbfs_get_obj(dat, sprite, NULL);

	self->sprite_graphics = spr_vram_load(image, char_tiles, char_size);
	memcpy(&SPRITE_PALETTE[16 * palette], char_pal, char_pal_len);

	Sprite_decode(&self->sprite, self->sprite_graphics, char_sprite);
	self->sprite.base->pos = pos;
}

void Character_decode(Character* self, Vector2 pos, const GBFS_FILE* dat, const u8* src)
{
	self->velocity = Vector2_create(0, 0);
	self->forward = Vector2_create(0, int_to_fx(-1));
	self->heading = NORTH;
	self->z = 0;
	
	u32 length;
	src += read_data(&length, src, sizeof length);

	char image[length + 1];
	src += read_string(image, src, length);

	src += read_data(&length, src, sizeof length);

	char pal[length + 1];
	src += read_string(pal, src, length);

	u32 char_size = 0;
	const TILE* char_tiles = gbfs_get_obj(dat, image, &char_size);

	u32 char_pal_len = 0;
	const u16* char_pal = gbfs_get_obj(dat, pal, &char_pal_len);

	self->sprite_graphics = spr_vram_load(image, char_tiles, char_size);

	Sprite_decode(&self->sprite, self->sprite_graphics, src);
	self->sprite.base->pos = pos;
	
	u32 shadow_size = 0;
	const TILE* shadow_tiles = gbfs_get_obj(dat, "shadow_test.tiles", &shadow_size);
	self->shadow_graphics = spr_vram_load("shadow_test.tiles", shadow_tiles, shadow_size);
	
	Sprite_construct(&self->shadow, pos, ATTR0_WIDE, ATTR1_SIZE_32, 14, self->shadow_graphics);
	self->shadow.base->oam.attr0 |= OBJ_MODE(1);
	self->shadow.base->mid.y = 0;
	self->shadow.base->half.y = 0;
	
	CpuFastSet(char_pal, &SPRITE_PALETTE[16 * self->sprite.pal], char_pal_len >> 2);
}

void Character_update(Character* self, FIXED dt)
{
	Vector2_plus_equal(&self->sprite.base->pos, Vector2_scalar_mult(self->velocity, dt));
}


void Character_draw(Character* self, FIXED offset_x, FIXED offset_y)
{
	Sprite_draw(&self->sprite, offset_x, offset_y + self->z);
	Sprite_draw(&self->shadow, offset_x, offset_y);
}

void Character_map_clamp(Character* self, Vector2 bounds)
{
	SPR_BASE* base = self->sprite.base;
	SPR_BASE* shadow = self->shadow.base;

	base->pos.x = clamp(0, bounds.x - 2 * base->half.x, base->pos.x);
	base->pos.y = clamp(0, bounds.y - 2 * base->half.y, base->pos.y);
	
	shadow->pos = base->pos;
	shadow->pos.x += base->mid.x - int_to_fx(16);
	shadow->pos.y += base->mid.y + int_to_fx(6);
}

void Character_destroy(Character* self)
{
	spr_vram_free(self->sprite_graphics);
	spr_vram_free(self->shadow_graphics);
	Sprite_destroy(&self->sprite);
	Sprite_destroy(&self->shadow);
}
