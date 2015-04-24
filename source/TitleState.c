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

typedef struct Mask
{
	const u16* graphics;
	u32 x, y;
	u32 width, height;
} Mask;

static Mask start_game = { NULL, 62, 83, 120, 16 };
static Mask options = { NULL, 82, 111, 80, 18 };

static bool item;
static bool item_prev;

static inline void mask_set(
	Mask mask, 
	bool on
	)
{
	u16* dst = (u16*)VRAM + mask.y * SCREEN_WIDTH + mask.x;
	u16 sign = on ? 1 : -1;

	u32 width = mask.width;
	u32 height = mask.height;

	const u16* src = mask.graphics;

	u32 j;
	for(j = 0; j < height; j++)
	{
		u32 i;
		for(i = 0; i < width; i++)
		{
			dst[j * SCREEN_WIDTH + i] += 
				sign * src[j * mask.width + i];
		}
	}
}

static void TitleState_construct(const GBFS_FILE* dat)
{
	SetMode( MODE_3 | BG2_ENABLE | OBJ_ENABLE | OBJ_1D_MAP );

	item = true;
	item_prev = true;

	u32 length;
	const u16* image = gbfs_get_obj(dat, "title_bg_menu.img.bin", &length);

	CpuFastSet(image, (u16*)VRAM, length >> 2);

	start_game.graphics = gbfs_get_obj(dat, "title_start_highlight.img.bin", NULL);
	options.graphics = gbfs_get_obj(dat, "title_opts_highlight.img.bin", NULL);

	mask_set(start_game, true);

	fade_in(float_to_fx(0.5f), 2, BIT(2));
}

static void TitleState_update(StateMachine* self, FIXED dt)
{
	if(key_hit(KEY_A))
	{
		STATE_TYPE next = item ? PLAY : TITLE; //TODO: change TITLE -> OPTIONS
		StateMachine_enter(self, next);
		return;
	}

	if(key_hit(KEY_UP | KEY_DOWN))
	{
		mask_set(options, item);
		item = !item;
		mask_set(start_game, item);
	}
}

static void TitleState_draw(void)
{
	
}

static void TitleState_destroy(void)
{
	fade_out(float_to_fx(0.5f), 3, BIT(2));
}

const STATE title_state =
{
	TitleState_construct,
	TitleState_update,
	TitleState_draw,
	TitleState_destroy
};