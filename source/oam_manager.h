#ifndef _oam_manager_h_
#define _oam_manager_h_

#include <gba_sprites.h>
#include "Vector2.h"

typedef struct SPR_BASE
{
	Vector2 pos;
	OBJATTR oam;
} SPR_BASE;

#define MAX_OBJ 128

void* spr_alloc(void);

void spr_free(void* ptr);

void init_oam(void);

void update_oam(void);

#endif
