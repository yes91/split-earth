#include "Character.h"

#include "tile.h"
#include "oam_manager.h"
#include <stddef.h>
#include <gbfs.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include "spr_vram_manager.h"
#include "filesys.h"

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
	char_size /= sizeof(TILE);

	u32 char_pal_len = 0;
	const u16* char_pal = gbfs_get_obj(dat, pal, &char_pal_len);

	const u8* char_sprite = gbfs_get_obj(dat, sprite, NULL);

	bool chached = false;
	self->sprite_graphics = spr_vram_alloc(image, &chached, sizeof(TILE) * char_size);
	if(!chached)
		tile_copy(spr_mem(self->sprite_graphics), char_tiles, char_size);
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

	CpuFastSet(char_pal, &SPRITE_PALETTE[16 * self->sprite.pal], char_pal_len >> 2);
}

void Character_draw(Character* self, FIXED offset_x, FIXED offset_y)
{
	Sprite_draw(&self->sprite, offset_x, offset_y + self->z);
}

void Character_destroy(Character* self)
{
	spr_vram_free(self->sprite_graphics);
	Sprite_destroy(&self->sprite);
}