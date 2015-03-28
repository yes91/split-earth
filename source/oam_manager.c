#include "oam_manager.h"

#include <gba_sprites.h>
#include <gba_types.h>
#include <string.h>
#include "debug.h"

/*OBJATTR obj_buffer[MAX_OBJ];
OBJAFFINE* obj_aff_buffer= (OBJAFFINE*)obj_buffer;*/

SPR_BASE sprites[MAX_OBJ];
u8 index_buffer[MAX_OBJ];

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

SPR_BASE* spr_alloc(void)
{    
	return alloc_stack_pop();
}

void spr_free(SPR_BASE* obj)
{
	obj->oam.attr0 = ATTR0_DISABLED;
	alloc_stack_push(obj);
}

void oam_copy(OBJATTR* dst, const SPR_BASE* src, u32 count)
{
	int i;
	for(i = 0; i < count; i++)
		dst[i] = src[index_buffer[i]].oam;
/* NOTE: while struct-copying is the Right Thing to do here, 
//   there's a strange bug in DKP that sometimes makes it not work
//   If you see problems, just use the word-copy version.
#if 1
    while(count--)
        *dst++ = (src++)->oam;
#else
    u32 *dstw= (u32*)dst, *srcw= (u32*)src;
    while(count--)
    {
        *dstw++ = *srcw++;
        *dstw++ = *srcw++;
    }
#endif*/

}

void oam_init(SPR_BASE* obj, u32 count)
{
    u32 n = count;
    SPR_BASE* dst = obj;
    // Hide each object
    while(n--)
    {
        dst->oam.attr0 = ATTR0_DISABLED;
        dst->oam.attr1 = 0;
        dst++;
    }

    int i;
    for(i = 0; i < MAX_OBJ; i++)
    	index_buffer[i] = i;
    // init oam
    oam_copy(OAM, obj, count);
}

void init_oam(void)
{
	oam_init(sprites, MAX_OBJ);

	int i;
	for(i = 0; i < MAX_OBJ; i++)
	{
		alloc_stack[i] = &sprites[i];
	}

	alloc_stack_end = MAX_OBJ;
}

int get_key(SPR_BASE sprite)
{
	return sprite.pos.y;
}

IWRAM_CODE void id_sort_shell(SPR_BASE sprites[], u8 ids[], int count)
{
    u32 ii, inc;
    // find initial 'inc' in sequence x[i+1]= 3*x[i]+1 ; x[1]=1
    for(inc=1; inc<=count; inc++)
        inc *= 3;
    // actual sort
    do
    {
        // division is done by reciprocal multiplication. So no worries.
        inc /= 3;                   // for ARM compile
        // inc = (inc*0x5556)>>16); // for THUMB compile
        for(ii=inc; ii<count; ii++)
        {
            u32 jj, id0= ids[ii]; 
            int key0= get_key(sprites[id0]);
            for(jj=ii; jj>=inc && get_key(sprites[ids[jj-inc]]) > key0; jj -= inc)
                ids[jj]= ids[jj-inc];
            ids[jj]= id0;
        }
    } while(inc > 1);
}

void update_oam(void)
{
	id_sort_shell(sprites, index_buffer, MAX_OBJ);
	oam_copy(OAM, sprites, MAX_OBJ);
}
