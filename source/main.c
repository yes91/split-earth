#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <pcx.h>
#include <stdio.h>
#include <stdlib.h>
#include "fastmath.h"
#include "Input.h"
#include "Sprite.h"
#include <gba_sprites.h>
#include "oam_manager.h"
#include "spr_vram_manager.h"
#include <string.h>
#include "debug.h"
#include "tile.h"
#include "Player.h"
#include "Camera.h"

#include "StateMachine.h"
#include "TitleState.h"
#include "PlayState.h"

#include <gbfs.h>

#define RGB16(r,g,b)  ((r)+(g<<5)+(b<<10))

// DEPRECATED
void consoleTest(const GBFS_FILE* dat)
{
	consoleDemoInit();

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH

	const char* text = NULL;
	u32 text_len = 0;

	text = gbfs_get_obj(dat, "test.txt", &text_len);
	
	iprintf("\x1b[5;5H%s\nSize of text in bytes: %u\n", text, text_len);

	while(1) { VBlankIntrWait(); }
}

// DEPRECATED
void imageTest(const GBFS_FILE* dat)
{
	SetMode( MODE_4 | BG2_ENABLE );

	const u8* image = gbfs_get_obj(dat, "splash.pcx", NULL);
	
	DecodePCX(image, (u16*)VRAM, BG_PALETTE);
	
	while(true) { VBlankIntrWait(); }
}


// DEPRECATED
void spriteTest(const GBFS_FILE* dat, FIXED dt)
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

	u16 metroid_sprite = spr_vram_load("metr.tiles", metr_tiles, metr_size);
	memcpy(SPRITE_PALETTE, metr_pal, metr_pal_len);

	Sprite sprite1;

	Sprite_construct(
		&sprite1, 
		Vector2_float(50.f, 50.f),
		ATTR0_SQUARE, 
		ATTR1_SIZE_64,
		0,
		metroid_sprite );

	u32 guy_size = 0;
	const TILE* guy_tiles = gbfs_get_obj(dat, "guy.tiles", &guy_size);
	u16 guy_sprite = spr_vram_load("guy.tiles", guy_tiles, guy_size);

	Sprite guys[32];
	int i;
	for (i = 0; i < 32; i++)
	{
		Sprite_construct(&guys[i], Vector2_float(0.f, 0.f), ATTR0_SQUARE, ATTR1_SIZE_32, 4, guy_sprite);
	}


	Player player;

	Player_load(
		&player,
		Vector2_float(100.f, 50.f),
		dat,
		"test_player.player"
		);

	u32 t = 0;

	u32 frame = 0;

	Camera cam = { 
		CAM_BG0, 
		{ 0, 0 }, 
		{ int_to_fx(256), int_to_fx(256) }, 
		&player.base.sprite 
	};

	while (1)
	{
		if(frame == 60)
		{
			debug_print("x: %d y: %d\n", fx_to_int(guys[1].base->pos.x), fx_to_int(guys[1].base->pos.y));
			frame = 0;
		}

		Player_update(&player, dt);

		sprite1.base->pos.x = int_to_fx(128 - 32 + (50 * fx_cos(t) >> 14));
		sprite1.base->pos.y = int_to_fx(128 - 32 + (50 * fx_sin(t) >> 14));				
		
		int grouping;
		for (i = 0; i < 32; i++)
		{
			grouping = i/8;
			guys[i].base->pos.x = int_to_fx(20+50*(grouping) + (-10 * (i%8) * fx_cos(t) >> 14));
			guys[i].base->pos.y = int_to_fx(20+50*(grouping) + (-10 * (i%8) * fx_sin(t) >> 14));
			Sprite_draw(&guys[i], cam.pos.x, cam.pos.y);
		}

		Camera_update(&cam);

		t = (t + 1) % 360;

		frame++;

		Sprite_draw(&sprite1, cam.pos.x, cam.pos.y);

		Player_draw(&player, cam.pos.x, cam.pos.y);

		update_oam();

		VBlankIntrWait();

		Camera_translate(&cam);
	}

	Player_destroy(&player);
	Sprite_destroy(&sprite1);
}

// DEPRECATED
void bgTest(const GBFS_FILE* dat){
	
	//Enable BG mode 0 (no affine BGs), 1D sprite mapping, sprites, and BG0 as visible)	
	SetMode( MODE_0 | OBJ_1D_MAP | OBJ_ON | BG0_ENABLE); 

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
	
	while(1){ }
}

//---------------------------------------------------------------------------------
void VblankInterrupt()
//---------------------------------------------------------------------------------

{
	key_poll();
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqSet(IRQ_VBLANK, VblankInterrupt);
	irqEnable(IRQ_VBLANK);

	const GBFS_FILE* dat = find_first_gbfs_file(find_first_gbfs_file);

	//Graphics context = Graphics_new(0);

	const FIXED dt = float_to_fx(1.f/60.f);

	init_oam();

	StateMachine sm;

	STATE state_table[] = {
		[TITLE] = title_state,
		[OPTION] = play_state,
		[PLAY] = play_state,
		[GAMEOVER] = play_state
	};

	StateMachine_construct(&sm, dat, state_table, TITLE);

	while(true)
	{
		StateMachine_update(&sm, dt);
		StateMachine_draw(&sm);
		VBlankIntrWait();
	}

	//boxTest(&context);
    //imageTest(dat);
	//consoleTest(dat, &context);
	//bgTest(dat, &context);
	
	//spriteTest(dat, dt);

	return 0;
}
