#include "Player.h"
#include "tile.h"
#include "Input.h"
#include "filesys.h"

#include <gbfs.h>
#include <gba_video.h>
#include <gba_systemcalls.h>

#include "fastmath.h"
#include "util.h"
#include "debug.h"
#include "oam_manager.h"
#include "spr_vram_manager.h"

void Player_construct(
	Player* self,
	FIXED x,
	FIXED y,
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

	u32 player_size = 0;
	const TILE* player_tiles = gbfs_get_obj(dat, image, &player_size);
	player_size /= sizeof(TILE);

	u32 player_pal_len = 0;
	const u16* player_pal = gbfs_get_obj(dat, pal, &player_pal_len);

	const u8* player_sprite = gbfs_get_obj(dat, sprite, NULL);

	self->sprite_graphics = spr_vram_alloc(image, sizeof(TILE) * player_size);
	tile_copy(spr_mem(self->sprite_graphics), player_tiles, player_size);
	memcpy(&SPRITE_PALETTE[16 * palette], player_pal, player_pal_len);

	Sprite_decode(&self->sprite, self->sprite_graphics, player_sprite);
	self->sprite.base->pos.x = x;
	self->sprite.base->pos.y = y;
}

void Player_load(Player* self, Vector2 pos, const struct GBFS_FILE* dat, const char* player)
{
	self->velocity = Vector2_create(0, 0);
	self->forward = Vector2_create(0, int_to_fx(-1));
	self->heading = NORTH;
	self->z = 0;
	
	const u8* src = gbfs_get_obj(dat, player, NULL);
	
	u32 length;
	src += read_data(&length, src, sizeof length);

	char image[length + 1];
	src += read_string(image, src, length);

	src += read_data(&length, src, sizeof length);

	char pal[length + 1];
	src += read_string(pal, src, length);

	u32 player_size = 0;
	const TILE* player_tiles = gbfs_get_obj(dat, image, &player_size);

	u32 player_pal_len = 0;
	const u16* player_pal = gbfs_get_obj(dat, pal, &player_pal_len);

	self->sprite_graphics = spr_vram_alloc(image, player_size);
	CpuFastSet(player_tiles, spr_mem(self->sprite_graphics), player_size >> 2);

	Sprite_decode(&self->sprite, self->sprite_graphics, src);
	self->sprite.base->pos = pos;

	CpuFastSet(player_pal, &SPRITE_PALETTE[16 * self->sprite.pal], player_pal_len >> 2);
}

static FIXED quad_interp(FIXED t, FIXED max)
{
	FIXED tmax = t * max;
	FIXED tsq = t * t;
	return ((4 << 16) / (max * max) * (tmax - tsq)) >> FIX_SHIFT;
}

static void walk(Player* self, Vector2 delta, FIXED speed)
{
	//2 px/f * 60 f/s = 120 px/s
	self->velocity = Vector2_scalar_mult(delta, speed); 

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
		self->forward = delta;
		Sprite_play(&self->sprite, new_heading);
	}
	else
	{
		Sprite_play(&self->sprite, new_heading - 4);
	}
}

static void jump(Player* self, u32 key, FIXED max, FIXED dt)
{
	static FIXED timer = 0;

	if(key)
	{
		timer += dt;
	}
	else
	{
		timer = 0;
		self->z = 0;
	}

	if(timer > 0 && timer <= max)
	{
		Vector2 backward = self->forward;
		Vector2_negate_equal(&backward);
		FIXED alpha = quad_interp(timer, max);
		self->velocity = Vector2_scalar_mult(backward, 300 * alpha);
		self->z = 10 * alpha;
	}
}

void Player_update(Player* self, FIXED dt)
{
	walk(self, key_dir(), int_to_fx(120));

	jump(self, key_held(KEY_R), float_to_fx(0.25f), dt);

	/*Vector2 target = Vector2_sub(self->sprite.pos, 
		Vector2_create(int_to_fx(100), int_to_fx(50)));

	Vector2_normalize(&target);

	debug_print("target / || target || = (%f, %f)\n", 
		fx_to_float(target.x), fx_to_float(target.y));*/

	Vector2_plus_equal(&self->sprite.base->pos, Vector2_scalar_mult(self->velocity, dt));

	if(key_hit(KEY_SELECT))
		self->sprite.pal -= 1;
}

void Player_draw(Player* self, FIXED offset_x, FIXED offset_y)
{
	Sprite_draw(&self->sprite, offset_x, offset_y + self->z);
}

void Player_destroy(Player* self)
{
	spr_vram_free(self->sprite_graphics);
	Sprite_destroy(&self->sprite);
}
