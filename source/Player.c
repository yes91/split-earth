#include "Player.h"
#include "Input.h"

#include <stddef.h>
#include <gbfs.h>
#include <gba_video.h>

#include "oam_manager.h"
#include "fastmath.h"
#include "util.h"
#include "debug.h"

void Player_construct(
	Player* self,
	Vector2 pos,
	const GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* sprite,
	int palette
	)
{
	Character_construct((Character*)self, pos, dat, image, pal, sprite, palette);
}

void Player_load(Player* self, Vector2 pos, const struct GBFS_FILE* dat, const char* player)
{
	const u8* src = gbfs_get_obj(dat, player, NULL);
	Character_decode((Character*)self, pos, dat, src);
}

static FIXED quad_interp(FIXED t, FIXED max)
{
	FIXED tmax = t * max;
	FIXED tsq = t * t;
	return ((4 << 16) / (max * max) * (tmax - tsq)) >> FIX_SHIFT;
}

static void walk(Character* self)
{
	//2 px/f * 60 f/s = 120 px/s

	Vector2 delta = self->velocity;
	Vector2_normalize(&delta);

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

static void jump(Character* self, u32 key, FIXED max, FIXED dt)
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
	Character* base = (Character*)self;

	base->velocity = Vector2_scalar_mult(key_dir(), int_to_fx(120)); 

	walk(base);

	jump(base, key_held(KEY_R), float_to_fx(0.25f), dt);

	/*Vector2 target = Vector2_sub(self->sprite.pos, 
		Vector2_create(int_to_fx(100), int_to_fx(50)));

	Vector2_normalize(&target);

	debug_print("target / || target || = (%f, %f)\n", 
		fx_to_float(target.x), fx_to_float(target.y));*/

	Vector2_plus_equal(&base->sprite.base->pos, Vector2_scalar_mult(base->velocity, dt));

	Character_update(base, dt);	

	if(key_hit(KEY_B))
		base->sprite.pal -= 1;
}

void Player_draw(Player* self, FIXED offset_x, FIXED offset_y)
{
	Character_draw((Character*)self, offset_x, offset_y);
}

void Player_destroy(Player* self)
{
	Character_destroy((Character*)self);
}
