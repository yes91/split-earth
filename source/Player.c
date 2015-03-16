#include "Player.h"
#include "Vector2.h"
#include "tile.h"
#include "tonc_input.h"
#include <string.h>

#include <gbfs.h>
#include <gba_video.h>

#include "debug.h"

void Player_construct(
	Player* self,
	const GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* anims,
	int palette,
	int slot
	)
{
	self->velocity = Vector2_create(0, 0);
	self->heading = NORTH;

	u32 player_size = 0;
	const TILE* player_tiles = gbfs_get_obj(dat, image, &player_size);
	player_size /= sizeof(TILE);

	u32 player_pal_len = 0;
	const u16* player_pal = gbfs_get_obj(dat, pal, &player_pal_len);

	const u8* player_ani = gbfs_get_obj(dat, anims, NULL);

    tile_copy(&tile_mem[4][slot], player_tiles, player_size);
    memcpy(&SPRITE_PALETTE[16 * palette], player_pal, player_pal_len);

    Sprite_construct(&self->sprite, int_to_fx(100), int_to_fx(50), ATTR0_SQUARE, ATTR1_SIZE_32, palette, slot);
	AnimContainer_decode(&self->sprite.anims, player_ani);
}

void Player_update(Player* self, FIXED dt)
{
	Vector2 delta = { int_to_fx(key_tri_horz()), int_to_fx(key_tri_vert()) };
	self->velocity = Vector2_scalar_mult(delta, int_to_fx(120)); //2 px/f * 60 f/s = 120 px/s

	Direction new_heading = self->heading;

	if(Vector2_mag_sq(&delta) > 0)
	{
		if(delta.x > 0)
			new_heading = EAST;
		else if(delta.x < 0)
			new_heading = WEST;
		else if(delta.y > 0)
			new_heading = SOUTH;
		else if(delta.y < 0)
			new_heading = NORTH;

		self->heading = new_heading;
		Sprite_play(&self->sprite, new_heading);
	}
	else
	{
		Sprite_play(&self->sprite, new_heading - 4);
	}

	FIXED dodge = 100 * int_to_fx(bit_tribool(key_held(-1), KI_R, KI_L));

	static FIXED timer = 0;

	if(dodge != 0)
	{
		timer += dt;
	}
	else
	{
		timer = 0;
	}

	Vector2 forward;
	switch(self->heading)
	{
		case NORTH: forward = Vector2_create(int_to_fx(0), int_to_fx(-1)); break;
		case SOUTH: forward = Vector2_create(int_to_fx(0), int_to_fx(1)); break;
		case EAST: forward = Vector2_create(int_to_fx(1), int_to_fx(0)); break;
		case WEST: forward = Vector2_create(int_to_fx(-1), int_to_fx(0)); break;
	}

	if(timer > 0 && timer < float_to_fx(0.5f))
		self->velocity = Vector2_scalar_mult(Vector2_perp(&forward), dodge);

	Vector2_plus_equal(&self->sprite.pos, Vector2_scalar_mult(self->velocity, dt));

	self->sprite.pal -= key_hit(KEY_SELECT);
}

void Player_draw(Player* self, int offset_x, int offset_y)
{
	Sprite_draw(&self->sprite, offset_x, offset_y);
}

void Player_destroy(Player* self)
{
	Sprite_destroy(&self->sprite);
}