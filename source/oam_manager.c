#include "oam_manager.h"

#include <gba_sprites.h>
#include <gba_types.h>
#include <string.h>
#include "debug.h"

OBJATTR obj_buffer[MAX_OBJ];
OBJAFFINE* obj_aff_buffer= (OBJAFFINE*)obj_buffer;

void* alloc_stack[MAX_OBJ];

u8 alloc_stack_end;

void* alloc_stack_pop(void)
{
	void* res = NULL;

	if(alloc_stack_end != 0) 
	{
		res = alloc_stack[alloc_stack_end - 1];

		debug_print("OAM stack end: %d\nAllocated pointer: %p\n", alloc_stack_end, res);

		alloc_stack_end--;
	}

	return res;
}

void alloc_stack_push(void* data)
{
	if(alloc_stack_end == MAX_OBJ) return;

	alloc_stack[alloc_stack_end] = data;
	alloc_stack_end++;

	debug_print("OAM stack end: %d\nFree'd pointer: %p\n", alloc_stack_end, data);
}

OBJATTR* obj_alloc(void)
{    
	return alloc_stack_pop();
}

void obj_free(OBJATTR* obj)
{
	obj->attr0 = ATTR0_DISABLED;
	alloc_stack_push(obj);
}

void oam_copy(OBJATTR* dst, const OBJATTR* src, u32 count)
{

// NOTE: while struct-copying is the Right Thing to do here, 
//   there's a strange bug in DKP that sometimes makes it not work
//   If you see problems, just use the word-copy version.
#if 1
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
    u32 n = count;
    u32 *dst = (u32*)obj;

    // Hide each object
    while(n--)
    {
        *dst++ = ATTR0_DISABLED;
        *dst++ = 0;
    }
    // init oam
    oam_copy(OAM, obj, count);
}

void init_oam(void)
{
	oam_init(obj_buffer, MAX_OBJ);

	int i;
	for(i = 0; i < MAX_OBJ; i++)
	{
		alloc_stack[i] = &obj_buffer[i];
	}

	alloc_stack_end = MAX_OBJ;
}

void update_oam(void)
{
	oam_copy(OAM, obj_buffer, MAX_OBJ);
}
