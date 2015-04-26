#include "PlayState.h"
#include "fixed.h"
#include "fastmath.h"
#include <gba_video.h>
#include <stddef.h>
#include <gbfs.h>

#include "tile.h"
#include "Player.h"
#include "Camera.h"
#include "Sprite.h"
#include "debug.h"
#include <gba_sprites.h>
#include "oam_manager.h"
#include "spr_vram_manager.h"
#include <string.h>
#include "Input.h"
#include "fade.h"
#include "Enemy.h"

#include "StateMachine.h"

static Player player;
static Enemy enemies[10];
static Camera cam;
static u32 t = 1;
static u32 frame = 0;

static void PlayState_draw(void);
static void PlayState_update(StateMachine*, FIXED);

static void PlayState_construct(const GBFS_FILE* dat)
{
	REG_BLDCNT = 3 << 6 | BIT(5) | BIT(4) | BIT(0);
	REG_BLDY = 0x10;

	SetMode( MODE_0 | OBJ_ENABLE | OBJ_1D_MAP | BG0_ENABLE );

	REG_BG0CNT = SCREEN_BASE(31);

	spr_vram_init(false);

	u32 tiles_size = 0;
	const TILE* bg_tiles = gbfs_get_obj(dat, "test_tileset.tiles", &tiles_size);
	tiles_size /= sizeof(TILE);
	
	u32 bg_pal_len = 0;
	const u16* bg_pal = gbfs_get_obj(dat, "test_tileset.pal", &bg_pal_len);
	
	u32 tilemap_size = 0;
	const u16* bg_map = gbfs_get_obj(dat, "BG_Maps.raw", &tilemap_size);
	
	memcpy(SCREEN_BASE_BLOCK(31), bg_map, tilemap_size);
	
	tile_copy(&tile_mem[0][0], bg_tiles, tiles_size);
	
	memcpy(BG_PALETTE, bg_pal, bg_pal_len);

	Player_load(
		&player,
		Vector2_float(100.f, 50.f),
		dat,
		"test_player.player"
		);

	Enemy_load(
		&enemies[0],
		Vector2_float(150.f, 50.f),
		dat,
		"skellyman.enemy",
		&player.base.sprite
		);

	Camera_construct(
		&cam, 
		CAM_BG0, 
		Vector2_float(0.f, 0.f), 
		Vector2_float(256.f, 256.f), 
		&player.base.sprite 
		);

	PlayState_update(NULL, 0);

	PlayState_draw();

	fade_in(float_to_fx(0.5f), 3,  BIT(5) | BIT(4) | BIT(0));

	REG_BLDCNT = 1 << 6 | BIT(0xD) | BIT(0x8);
	u32 alpha = 0xD;
	REG_BLDALPHA = (0x10 - alpha) << 9 | alpha;
}

static void PlayState_update(StateMachine* sm, FIXED dt)
{
	Camera_translate(&cam);

	if(frame == 60)
	{
		//debug_print("x: %d y: %d\n", fx_to_int(guys[1].base->pos.x), fx_to_int(guys[1].base->pos.y));
		frame = 0;
	}

	Player_update(&player, dt);
	Character_map_clamp((Character*)&player, cam.bounds);
	Enemy_update(&enemies[0], dt);
	Character_map_clamp((Character*)&enemies[0], cam.bounds);
	
	if(key_hit(KEY_A))
	{
		StateMachine_enter(sm, TITLE);
		return;
	}

	Camera_update(&cam);

	t = (t + 1) % 360;
	
	if (check_collision(&((Character*)&player)->sprite, &((Character*)&enemies[0])->sprite))
	{
		debug_print("YEAH!%i\n", 1);
	}
	frame++;

}

static void PlayState_draw(void)
{
	Player_draw(&player, cam.pos.x, cam.pos.y);

	Enemy_draw(&enemies[0], cam.pos.x, cam.pos.y);

	update_oam();
}

static void PlayState_destroy(void)
{
	fade_out(float_to_fx(0.5f), 3, BIT(5) | BIT(4) | BIT(0));

	Player_destroy(&player);
	Enemy_destroy(&enemies[0]);
}

int check_collision(Sprite* a, Sprite* b)
{
	Vector2 a_tl = *Sprite_pos(a);
	Vector2 a_br = Vector2_add(a_tl, Vector2_add(Sprite_mid(a), Sprite_half(a)));
	Vector2 b_tl = *Sprite_pos(b);	
	Vector2 b_br = Vector2_add(b_tl, Vector2_add(Sprite_mid(b), Sprite_half(b)));
	return !( (a_br.x < b_tl.x) || (a_tl.x > b_br.x) || (a_tl.y > b_br.y) || (a_br.y < b_tl.y) );
}

const STATE play_state =
{
	PlayState_construct,
	PlayState_update,
	PlayState_draw,
	PlayState_destroy
};
