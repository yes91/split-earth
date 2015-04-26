#include "oam_manager.h"

#include <gba_sprites.h>
#include <gba_types.h>
#include <string.h>
#include "debug.h"

#include "PoolAllocator.h"

static SPR_BASE sprite_mem[MAX_OBJ];

static PoolAllocator sprite_pool;

static u8 index_buffer[MAX_OBJ];

/**	@fn spr_alloc
 *	@brief Allocate the pool of sprites 
 */
void* spr_alloc(void)
{   
	return PoolAllocator_alloc(&sprite_pool);
}

/**	@fn spr_free
 *	@brief Frees up the space in the pool occupied by a sprite
 *	@param *ptr Pointer to sprite
 */
void spr_free(void* ptr)
{
	SPR_BASE* sprite = ptr;
	sprite->oam.attr0 = ATTR0_DISABLED;
	PoolAllocator_free(&sprite_pool, ptr);
}

/**	@fn oam_copy
 *	@brief Copies OAM data to the appropriate OAM space
 *	@param *dst OBJATTR pointer to OAM destination
 *	@param *src SPR_BASE pointer to the location of sprites
 *	@param count How many sprite's OAM data to be copied
 */
void oam_copy(OBJATTR* dst, const SPR_BASE* src, u32 count)
{
	int i;
	for(i = 0; i < count; i++)
		dst[i] = src[index_buffer[i]].oam;
}

/**	@fn oam_init
 *	@brief Initializes the OAM
 *	@param *obj SPR_BASE pointer for where OAM memory starts
 *	@param count How many OAM entries to initialize
 *	This sets all current OAM entries to disabled, then copies over the current sprite OAM data
 */
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

/**	@fn init_oam
 *	@brief Calls for the OAM to be initialized and for the creation of a sprite pool
 *	@see oam_init
 */
void init_oam(void)
{
	oam_init(sprite_mem, MAX_OBJ);
	PoolAllocator_construct(&sprite_pool, sprite_mem, MAX_OBJ, sizeof(SPR_BASE));
}

/**	@fn get_key
 *	@brief Key function for the OAM sorting functions
 *	@param *sprite SPR_BASE pointer to the sprite whose y value will be checked
 *	@return Y-value for the bottom of the sprite's bounding box
 *	@see id_sort_insertion
 *	@see id_sort_shell
 */
int get_key(SPR_BASE* sprite)
{
	return sprite->pos.y + sprite->mid.y + sprite->half.y;
}


/**	@fn id_sort_insertion
 *	@brief Insertion sort for OAM entries
 *	@param sprites[] SPR_BASE array where all sprites reside
 *	@param ids[] u8 array containing IDs that becomes sorted
 *	@param count How many sprites need to be sorted
 */
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

/**	@fn id_sort_shell
 *	@brief Shell sort for OAM entries
 *	@param sprites[] SPR_BASE array where all sprites reside
 *	@param ids[] u8 array containing IDs that becomes sorted
 *	@param count How many sprites need to be sorted
 */

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

/**	@fn update_oam
 *	@brief Executed every frame, calls the OAM sorting function along with the copy
 */
void update_oam(void)
{
	id_sort_insertion(sprite_mem, index_buffer, MAX_OBJ);
	oam_copy(OAM, sprite_mem, MAX_OBJ);
}
