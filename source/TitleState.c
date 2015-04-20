#include "TitleState.h"
#include <gba_video.h>
#include <stddef.h>
#include <gbfs.h>
#include <gba_systemcalls.h>
#include <gba_sprites.h>
#include <string.h>

#include "Sprite.h"
#include "spr_vram_manager.h"
#include "oam_manager.h"
#include "tile.h"

#include "StateMachine.h"
#include "Input.h"
#include "debug.h"

static const u16* start_highlight;
static const u16* opts_highlight;

static bool item;
static bool item_prev;

static void start_set(bool);

static void opts_set(bool);

static void TitleState_construct(const GBFS_FILE* dat)
{
	SetMode( MODE_3 | BG2_ENABLE | OBJ_ENABLE | OBJ_1D_MAP );

	item = true;
	item_prev = true;

	u32 length;
	const u8* image = gbfs_get_obj(dat, "title_bg_menu.img.bin", &length);

	CpuFastSet(image, (u16*)VRAM, length >> 2);

	u32 start_size;
	start_highlight = gbfs_get_obj(dat, "title_start_highlight.img.bin", &start_size);
	opts_highlight = gbfs_get_obj(dat, "title_opts_highlight.img.bin", NULL);

	debug_print("start_size == %d\n", start_size / sizeof(u16));

	start_set(true);
}

static void start_set(bool on)
{
	u16* dst = (u16*)VRAM + 83 * SCREEN_WIDTH + 62;
	u16 sign = on ? 1 : -1;

	u32 x;
	for(x = 0; x < 120; x++)
	{
		u32 y;
		for(y = 0; y < 16; y++)
		{
			dst[y * SCREEN_WIDTH + x] += sign * start_highlight[y * 120 + x];
		}		
	}
}

static void opts_set(bool on)
{
	u16* dst = (u16*)VRAM + 111 * SCREEN_WIDTH + 82;
	u16 sign = on ? 1 : -1;

	u32 x;
	for(x = 0; x < 80; x++)
	{
		u32 y;
		for(y = 0; y < 18; y++)
		{
			dst[y * SCREEN_WIDTH + x] += sign * opts_highlight[y * 80 + x];
		}		
	}
}

static void TitleState_update(StateMachine* self, FIXED dt)
{
	if(item && key_hit(KEY_A))
	{
		StateMachine_enter(self, PLAY);
		return;
	}

	if(key_hit(KEY_UP | KEY_DOWN))
	{
		item = !item;
	}
}

static void TitleState_draw(void)
{
	if(item && !item_prev)
	{
		opts_set(false);
		start_set(true);
		item_prev = true;
	}
	else if (!item && item_prev)
	{
		start_set(false);
		opts_set(true);
		item_prev = false;
	}
}

static void TitleState_destroy(void)
{
	
}

const STATE title_state =
{
	TitleState_construct,
	TitleState_update,
	TitleState_draw,
	TitleState_destroy
};