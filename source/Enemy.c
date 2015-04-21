#include "Enemy.h"
#include "Input.h"

#include <stddef.h>
#include <gbfs.h>
#include <gba_video.h>

#include "oam_manager.h"
#include "fastmath.h"
#include "util.h"
#include "debug.h"

#include <stdlib.h>

void Enemy_construct(
	Enemy* self,
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

void Enemy_load(Enemy* self, Vector2 pos, const struct GBFS_FILE* dat, const char* enemy)
{
	const u8* src = gbfs_get_obj(dat, enemy, NULL);
	Character_decode((Character*)self, pos, dat, src);
}

/*static FIXED quad_interp(FIXED t, FIXED max)
{
	FIXED tmax = t * max;
	FIXED tsq = t * t;
	return ((4 << 16) / (max * max) * (tmax - tsq)) >> FIX_SHIFT;
}*/

static void walk(Character* self, Vector2 delta, FIXED speed)
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


// from tonc_core.h/.c
// A Quick (and dirty) random number generator and its seeder

int __qran_seed= 42;     // Seed / rnd holder

// Seed routine
int sqran(int seed)
{	
	int old= __qran_seed;
	__qran_seed= seed; 
	return old;
} 

//! Quick (and very dirty) pseudo-random number generator 
/*! \return random in range [0,8000h>
*/
static inline int qran(void)
{	
	__qran_seed= 1664525*__qran_seed+1013904223;
	return (__qran_seed>>16) & 0x7FFF;
}

void Enemy_update(Enemy* self, FIXED dt)
{
	static FIXED move_time = 0;
	
	Character* base = (Character*)self;
	
	Vector2 delta = base->forward;

	if(move_time <= 0)
	{
		delta.x = qran() - 0x4000;
		delta.y = qran() - 0x4000;

		move_time = float_to_fx(0.5f);
	}

	Vector2_normalize(&delta);

	walk(base, delta, int_to_fx(120));

	move_time -= dt;

	/*Vector2 target = Vector2_sub(self->sprite.pos, 
		Vector2_create(int_to_fx(100), int_to_fx(50)));

	Vector2_normalize(&target);

	debug_print("target / || target || = (%f, %f)\n", 
		fx_to_float(target.x), fx_to_float(target.y));*/

	Vector2_plus_equal(&base->sprite.base->pos, Vector2_scalar_mult(base->velocity, dt));

	if(key_hit(KEY_B))
		base->sprite.pal -= 1;
}


void Enemy_draw(Enemy* self, FIXED offset_x, FIXED offset_y)
{
	Character_draw((Character*)self, offset_x, offset_y);
}

void Enemy_destroy(Enemy* self)
{
	Character_destroy((Character*)self);
}

