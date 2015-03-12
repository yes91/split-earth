#include "Player.h"
#include "Vector2.h"
#include "tile.h"
#include "tonc_input.h"
#include <string.h>

#include <gbfs.h>
#include <gba_video.h>

void Player_construct(
	Player* self,
	GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* anims,
	int palette,
	int slot
	)
{
	u32 player_size = 0;
	const TILE* player_tiles = gbfs_get_obj(dat, image, &player_size);
	player_size /= sizeof(TILE);

	u32 player_pal_len = 0;
	const u16* player_pal = gbfs_get_obj(dat, pal, &player_pal_len);

	const u8* player_ani = gbfs_get_obj(dat, anims, NULL);

    tile_copy(&tile_mem[4][slot], player_tiles, player_size);
    memcpy(&SPRITE_PALETTE[16 * palette], player_pal, player_pal_len);

    Sprite_construct(&self->sprite, 100, 50, ATTR0_SQUARE, ATTR1_SIZE_32, palette, slot);
	AnimContainer_decode(&self->sprite.anims, player_ani);
}

void Player_update(Player* self)
{
	Vector2 delta = {key_tri_horz(), key_tri_vert()};

	Direction new_heading = self->heading;

	if(Vector2_mag_sq(&delta) > 0)
	{
		switch(delta.x)
		{
			case 1: new_heading = EAST; break;
			case -1: new_heading = WEST; break;
			default:
				switch(delta.y)
				{
					case 1: new_heading = SOUTH; break;
					case -1: new_heading = NORTH; break;
				} 
		}

		self->heading = new_heading;
		Sprite_play(&self->sprite, new_heading);
	}
	else
	{
		Sprite_play(&self->sprite, new_heading - 4);
	}

	self->sprite.x += 2 * delta.x;
	self->sprite.y += 2 * delta.y;

	self->sprite.pal += bit_tribool(key_hit(-1), KI_R, KI_L);

	if(key_hit(KEY_A))
	{
		self->sprite.oam->attr1 ^= ATTR1_FLIP_X;
	}

	if(key_hit(KEY_B))
	{
		self->sprite.oam->attr1 ^= ATTR1_FLIP_Y;
	}
}

void Player_draw(Player* self)
{
	Sprite_draw(&self->sprite);
}

void Player_destroy(Player* self)
{
	Sprite_destroy(&self->sprite);
}