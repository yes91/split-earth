#ifndef _spr_vram_manager_
#define _spr_vram_manager_

#include <gba_types.h>

void spr_vram_init(void);

void* spr_mem(u16 ptr);

u16 spr_vram_alloc(u32 size);

void spr_vram_free(u16 ptr);

#endif