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
#include "fixed.h"

#include "StateMachine.h"
#include "Input.h"
#include "debug.h"
#include "fade.h"

#define START_X 62
#define START_Y 83
#define START_WIDTH 120
#define START_HEIGHT 16
static const u16* start_highlight;

#define OPTS_X 82
#define OPTS_Y 111
#define OPTS_WIDTH 80
#define OPTS_HEIGHT 18
static const u16* opts_highlight;

static bool item;
static bool item_prev;

static inline void mask_set(
	const u16* mask, 
	bool on, 
	u32 x, 
	u32 y, 
	u32 width, 
	u32 height
	)
{
	u16* dst = (u16*)VRAM + y * SCREEN_WIDTH + x;
	u16 sign = on ? 1 : -1;

	u32 i;
	for(i = 0; i < width; i++)
	{
		u32 j;
		for(j = 0; j < height; j++)
		{
			dst[j * SCREEN_WIDTH + i] += 
				sign * mask[j * width + i];
		}		
	}
}

static inline void start_set(bool on)
{
	mask_set(start_highlight, on, START_X, START_Y, START_WIDTH, START_HEIGHT);
}

static inline void opts_set(bool on)
{
	mask_set(opts_highlight, on, OPTS_X, OPTS_Y, OPTS_WIDTH, OPTS_HEIGHT);
}

static void TitleState_construct(const GBFS_FILE* dat)
{
	SetMode( MODE_3 | BG2_ENABLE | OBJ_ENABLE | OBJ_1D_MAP );

	item = true;
	item_prev = true;

	u32 length;
	const u8* image = gbfs_get_obj(dat, "title_bg_menu.img.bin", &length);

	CpuFastSet(image, (u16*)VRAM, length >> 2);

	start_highlight = gbfs_get_obj(dat, "title_start_highlight.img.bin", NULL);
	opts_highlight = gbfs_get_obj(dat, "title_opts_highlight.img.bin", NULL);

	start_set(true);

	fade_in(float_to_fx(1.0f), 3, BIT(2));
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
	fade_out(float_to_fx(1.0f), 3, BIT(2));
}

const STATE title_state =
{
	TitleState_construct,
	TitleState_update,
	TitleState_draw,
	TitleState_destroy
};