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

#include "StateMachine.h"

static Player player;
static u16 metroid_sprite;
static Sprite sprite1;
static u16 guy_sprite;
static Sprite guys[32];
static Camera cam;
static u32 t = 0;
static u32 frame = 0;

static void PlayState_construct(const GBFS_FILE* dat)
{
    SetMode( MODE_0 | OBJ_ENABLE | OBJ_1D_MAP | BG0_ENABLE );

	REG_BG0CNT = SCREEN_BASE(31);

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

	u32 metr_size = 0;
	const TILE* metr_tiles = gbfs_get_obj(dat, "metr.tiles", &metr_size);

	u32 metr_pal_len = 0;
	const u16* metr_pal = gbfs_get_obj(dat, "metr.pal", &metr_pal_len);

	metroid_sprite = spr_vram_load("metr.tiles", metr_tiles, metr_size);
	memcpy(SPRITE_PALETTE, metr_pal, metr_pal_len);

	Sprite_construct(
		&sprite1, 
		Vector2_float(50.f, 50.f),
		ATTR0_SQUARE, 
		ATTR1_SIZE_64,
		0,
		metroid_sprite 
		);

	u32 guy_size = 0;
	const TILE* guy_tiles = gbfs_get_obj(dat, "guy.tiles", &guy_size);
	guy_sprite = spr_vram_load("guy.tiles", guy_tiles, guy_size);

	u32 i;
	for (i = 0; i < 32; i++)
	{
		Sprite_construct(
			&guys[i], 
			Vector2_float(0.f, 0.f), 
			ATTR0_SQUARE, 
			ATTR1_SIZE_32, 
			3, 
			guy_sprite
			);
	}

	Player_load(
		&player,
		Vector2_float(100.f, 50.f),
		dat,
		"test_player.player"
		);

	Camera_construct(
		&cam, 
		CAM_BG0, 
		Vector2_float(0.f, 0.f), 
		Vector2_float(256.f, 256.f), 
		&player.base.sprite 
		);
}

static void PlayState_update(StateMachine* sm, FIXED dt)
{
	Camera_translate(&cam);

	if(frame == 60)
	{
		debug_print("x: %d y: %d\n", fx_to_int(guys[1].base->pos.x), fx_to_int(guys[1].base->pos.y));
		frame = 0;
	}

	Player_update(&player, dt);

	sprite1.base->pos.x = int_to_fx(128 - 32 + (50 * fx_cos(t) >> 14));
	sprite1.base->pos.y = int_to_fx(128 - 32 + (50 * fx_sin(t) >> 14));				
	
	int grouping;
	int i;
	for (i = 0; i < 32; i++)
	{
		grouping = i/8;
		guys[i].base->pos.x = int_to_fx(20 + 50 * grouping + (-10 * (i%8) * fx_cos(t) >> 14));
		guys[i].base->pos.y = int_to_fx(20 + 50 * grouping + (-10 * (i%8) * fx_sin(t) >> 14));
	}

	if(key_hit(KEY_A))
	{
		StateMachine_enter(sm, TITLE);
		return;
	}

	Camera_update(&cam);

	t = (t + 1) % 360;

	frame++;

}

static void PlayState_draw(void)
{
	u32 i;
	for(i = 0; i < 32; i++)
		Sprite_draw(&guys[i], cam.pos.x, cam.pos.y);

    Sprite_draw(&sprite1, cam.pos.x, cam.pos.y);

	Player_draw(&player, cam.pos.x, cam.pos.y);

	update_oam();
}

static void PlayState_destroy(void)
{
    spr_vram_free(metroid_sprite);
    spr_vram_free(guy_sprite);
    Player_destroy(&player);
    Sprite_destroy(&sprite1);

    u32 i;
	for(i = 0; i < 32; i++)
		Sprite_destroy(&guys[i]);
}

const STATE play_state =
{
    PlayState_construct,
    PlayState_update,
    PlayState_draw,
    PlayState_destroy
};
