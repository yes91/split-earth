#include "oam_manager.h"

#include <gba_sprites.h>
#include <gba_types.h>
#include <string.h>
#include "debug.h"

/*OBJATTR obj_buffer[MAX_OBJ];
OBJAFFINE* obj_aff_buffer= (OBJAFFINE*)obj_buffer;*/

typedef union Block // Sprite memory block
{
	union Block * next;
	SPR_BASE sprite;
} Block;

static Block sprite_mem[MAX_OBJ];

static Block* head = NULL;

static u8 index_buffer[MAX_OBJ];

void* spr_alloc(void)
{   
	void* result = head;
	debug_print("Allocated pointer: %p\n", result);
	head = head->next;
	return result;
}

void spr_free(void* ptr)
{
	Block* block = ptr;
	block->sprite.oam.attr0 = ATTR0_DISABLED;
	block->next = head;
	head = block;
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
	oam_init((SPR_BASE*)sprite_mem, MAX_OBJ);

	head = sprite_mem;
	Block* current = head;
	int i;
	for(i = 1; i < MAX_OBJ; i++)
	{
		current->next = &sprite_mem[i];
		current = current->next;
	}

	current->next = NULL;
}

int get_key(SPR_BASE* sprite)
{
	return sprite->pos.y + sprite->mid.y + sprite->half.y;
}

IWRAM_CODE void id_sort_insertion(SPR_BASE sprites[], u8 ids[], int count)
{
	u32 i;
	for(i = 1; i < count; i++)
	{
		u8 id = ids[i];
    	int key = get_key(sprites + id);
    	u32 j = i;
    	while(j > 0 && get_key(sprites + ids[j-1]) < key)
    	{
        	ids[j] = ids[j-1];
        	j--;
        }

    	ids[j] = id;
 	}
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
            int key0= get_key(sprites + id0);
            for(jj=ii; jj>=inc && get_key(sprites + ids[jj-inc]) < key0; jj -= inc)
                ids[jj]= ids[jj-inc];
            ids[jj]= id0;
        }
    } while(inc > 1);
}

void update_oam(void)
{
	id_sort_insertion((SPR_BASE*)sprite_mem, index_buffer, MAX_OBJ);
	oam_copy(OAM, (SPR_BASE*)sprite_mem, MAX_OBJ);
}
