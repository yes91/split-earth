#include "oam_manager.h"

#include <gba_sprites.h>
#include <gba_types.h>
#include <string.h>

OBJATTR obj_buffer[MAX_OBJ];
OBJAFFINE* obj_aff_buffer= (OBJAFFINE*)obj_buffer;

typedef enum OBJ_ALLOC_STATUS
{
	USED,
	FREE
} OBJ_ALLOC_STATUS;

OBJ_ALLOC_STATUS obj_alloc_map[MAX_OBJ];

void* obj_alloc(void)
{
	int i;
	for(i = 0; i <= MAX_OBJ && obj_alloc_map[i] != FREE; i++);
	
	if(i != MAX_OBJ)
	{
		return &obj_buffer[i];
	}

	return NULL;
}

void obj_free(void* oam)
{
	u8 index = (u8)((oam - (void*)obj_buffer) / sizeof(OBJATTR));

	if(obj_alloc_map[index] != USED) return;

	obj_alloc_map[index] = FREE;
}

void oam_copy(OBJATTR* dst, const OBJATTR* src, u32 count)
{

// NOTE: while struct-copying is the Right Thing to do here, 
//   there's a strange bug in DKP that sometimes makes it not work
//   If you see problems, just use the word-copy version.
#if 0
    while(count--)
        *dst++ = *src++;
#else
    u32 *dstw= (u32*)dst, *srcw= (u32*)src;
    while(count--)
    {
        *dstw++ = *srcw++;
        *dstw++ = *srcw++;
    }
#endif

}

void oam_init(OBJATTR* obj, u32 count)
{
    u32 nn= count;
    u32 *dst= (u32*)obj;

    // Hide each object
    while(nn--)
    {
        *dst++= ATTR0_DISABLED;
        *dst++= 0;
    }
    // init oam
    oam_copy(OAM, obj, count);
}

void init_oam(void)
{
	oam_init(obj_buffer, MAX_OBJ);
	memset(obj_alloc_map, FREE, MAX_OBJ);
}

void update_oam(void)
{
	oam_copy(OAM, obj_buffer, MAX_OBJ);
}
