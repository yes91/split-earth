#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <pcx.h>
#include <stdio.h>
#include <stdlib.h>
#include "fastmath.h"
#include "Graphics.h"
#include "tonc_input.h"
#include "plot.h"
#include "Sprite.h"
#include <gba_sprites.h>
#include "oam_manager.h"
#include <string.h>
#include "debug.h"
#include "tile.h"
#include "Player.h"
#include "Camera.h"

#include <gbfs.h>

#define RGB16(r,g,b)  ((r)+(g<<5)+(b<<10))

void WaitForVblank(void)
{
	while (REG_VCOUNT >= 160);   // wait till VDraw
	while (REG_VCOUNT < 160);    // wait till VBlank
}

typedef struct
{

	int x, y;

}sPoint2D;



typedef struct
{

	int x, y;

	sPoint2D p[4];

	u8 color;

	int angle;
}sBox;

void rotate(sBox* box, int amount)
{
	box->angle += amount;	

	if(box->angle < 0) box->angle += 360;

	box->angle %= 360;
}

void drawBox(Graphics* g, sBox* box)
{

	int angle = box->angle;

	sPoint2D p[4];
	
	int i;
	for(i = 0; i < 4; i++) {
		p[i].x = ( ( cos(angle) * (box->p[i].x >> 14) - sin(angle) * (box->p[i].y >> 14) ) ) + box->x;
		p[i].y = ( ( sin(angle) * (box->p[i].x >> 14) + cos(angle) * (box->p[i].y >> 14) ) ) + box->y;
	}

 
	slow_line(p[0].x >> 14, p[0].y >> 14, p[1].x >> 14, p[1].y >> 14, box->color, g);

	slow_line(p[1].x >> 14, p[1].y >> 14, p[2].x >> 14, p[2].y >> 14, box->color, g);

	slow_line(p[2].x >> 14, p[2].y >> 14, p[3].x >> 14, p[3].y >> 14, box->color, g);

	slow_line(p[3].x >> 14, p[3].y >> 14, p[0].x >> 14, p[0].y >> 14, box->color, g);
}

//WaitForVblank and Draw line not shown...code is same as before

int fixed_mul(int fa, int fb, int shift)
{
	return (fa * fb) >> shift;
}

void boxTest(Graphics* context)
{
	Graphics_setMode(context, MODE_4 | BG2_ENABLE);

	sBox box = {

		50 << 14, 40 << 14, //x,y

		{

			{ -20 << 14, 10 << 14 }, //points

			{ 20 << 14, 10 << 14 },

			{ 20 << 14, -10  << 14},

			{ -20 << 14, -10 << 14 }

		},

	        2, //color

		90 //angle

	};

	context->paletteBuffer[1] = RGB16(20, 5, 20);
	context->paletteBuffer[2] = RGB16(0, 31, 31);

	while (1)
	{
		clearScreen(context, 1);

		drawBox(context, &box);
		
		rotate(&box, 2 * key_tri_horz());

		int forward = key_tri_vert();

		if(key_held(KEY_A)) forward *= 3;

		box.x += forward * cos(box.angle);
		box.y += forward * sin(box.angle);

		VBlankIntrWait();

		flip(context);
	}
}

void consoleTest(const GBFS_FILE* dat, Graphics* context)
{
	Graphics_setMode(context, 0);

	consoleDemoInit();

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH

	const char * text = NULL;
	u32 text_len = 0;

	text = gbfs_get_obj(dat, "test.txt", &text_len);
	
	iprintf("\x1b[5;5H%s\nSize of text in bytes: %u\n", text, text_len);

	while(1) { VBlankIntrWait(); }
}

void imageTest(const GBFS_FILE* dat, Graphics* context)
{
	Graphics_setMode(context, MODE_4 | BG2_ENABLE);

	const u8* image = gbfs_get_obj(dat, "splash.pcx", NULL);
	
	DecodePCX(image, (u16*)context->currentBuffer, (u16*)context->paletteBuffer);

	flip(context);
	
	while(1) { VBlankIntrWait(); }
}



void spriteTest(const GBFS_FILE* dat, Graphics* context, FIXED dt)
{
	Graphics_setMode(context, MODE_0 | OBJ_ENABLE | OBJ_1D_MAP | BG0_ENABLE );

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
	metr_size /= sizeof(TILE);

	u32 metr_pal_len = 0;
	const u16* metr_pal = gbfs_get_obj(dat, "metr.pal", &metr_pal_len);

	tile_copy(&tile_mem[4][0], metr_tiles, metr_size);
    memcpy(SPRITE_PALETTE, metr_pal, metr_pal_len);

    Sprite sprite1;

	Sprite_construct(&sprite1, 50, 50, ATTR0_SQUARE, ATTR1_SIZE_64, 0, 0);

	Player player;

	Player_construct(&player, dat, "guy.tiles", "guy.pal", "guy.ani", 6, metr_size);

	u32 t = 0;

	u32 frame = 0;

	Camera cam = { CAM_BG0, { 0, 0 }, { int_to_fx(256), int_to_fx(256) }, &player.sprite };

	while (1)
	{
		if(frame == 60)
		{
			debug_print("x: %d y: %d\n", fx_to_int(cam.pos.x), fx_to_int(cam.pos.y));
			frame = 0;
		}

		Player_update(&player, dt);

		sprite1.pos.x = int_to_fx(128 - 32 + (50 * cos(t) >> 14));
		sprite1.pos.y = int_to_fx(128 - 32 + (50 * sin(t) >> 14));

		Camera_update(&cam);

		t++;

		if(t > 360) t = 0;

		frame++;

		Sprite_draw(&sprite1, cam.pos.x, cam.pos.y);

		Player_draw(&player, cam.pos.x, cam.pos.y);

		VBlankIntrWait();

		Camera_translate(&cam);
	}

	Player_destroy(&player);
	Sprite_destroy(&sprite1);
}

void bgTest(const GBFS_FILE* dat, Graphics* context){
	
	Graphics_setMode(context, MODE_0 | OBJ_1D_MAP | OBJ_ON | BG0_ENABLE); //Enable BG mode 0 (no affine BGs), 1D sprite mapping, sprites, and BG0 as visible)
	
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
	update_oam();
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

	Graphics context = Graphics_new(0);

	init_oam();

	const FIXED dt = float_to_fx(1.f/60.f);

	//boxTest(&context);
    //imageTest(dat, &context);
	//consoleTest(dat, &context);
	//bgTest(dat, &context);
	spriteTest(dat, &context, dt);

	return 0;
}
