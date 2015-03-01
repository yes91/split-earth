#ifndef _oam_manager_h_
#define _oam_manager_h_

#include <gba_sprites.h>

#define MAX_OBJ 128

void* obj_alloc(void);

void obj_free(void* oam);

void init_oam(void);

void update_oam(void);

#endif
