#include "TitleState.h"
#include <gba_video.h>
#include <stddef.h>
#include <gbfs.h>
#include <pcx.h>
#include "StateMachine.h"
#include "Input.h"

static void TitleState_construct(const GBFS_FILE* dat)
{
	SetMode( MODE_4 | BG2_ENABLE );

	const u8* image = gbfs_get_obj(dat, "splash.pcx", NULL);
	
	DecodePCX(image, (u16*)VRAM, BG_PALETTE);
}

static void TitleState_update(StateMachine* self, FIXED dt)
{
	if(key_hit(KEY_A))
	{
		StateMachine_enter(self, PLAY);
		return;
	}
}

static void TitleState_draw(void)
{

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