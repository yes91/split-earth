#include "spr_vram_manager.h"
#include <gba_sprites.h>
#include <gba_systemcalls.h>
#include "tile.h"
#include "debug.h"
#include "hash.h"

#define PTR_ALIGN(size) ((((size) + (sizeof(TILE)-1)) & ~(sizeof(TILE)-1)) / sizeof(TILE))
#define BLOCKS 1024

static u16 offset = 0;
static TILE* base = &tile_mem[4][0];

typedef enum ALLOC_STATUS
{	
	FREE,
	USED,
	CONTINUE
} ALLOC_STATUS;

typedef struct ALLOC_ENTRY 
{
	ALLOC_STATUS status;
	u32 hash;
	u32 ref_count;
} ALLOC_ENTRY;

static ALLOC_ENTRY alloc_map[BLOCKS];

void spr_vram_init(bool bitmapMode)
{
	offset = bitmapMode ? 512 : 0;

	u32 i;
	for(i = offset; i < BLOCKS; i++)
		alloc_map[i].status = FREE;
}

void* spr_mem(u16 ptr)
{
	return base + ptr;
}

static u16 next_used(u16 start)
{
	u32 i;
	for(i = start; i < BLOCKS && alloc_map[i].status != USED; i++);
	return i;
}

static u16 find_match(u32 hash)
{
	u16 used_index = next_used(offset);

	while(used_index < BLOCKS)
	{
		if(alloc_map[used_index].hash == hash)
		{
			return used_index;
		}

		used_index = next_used(used_index + 1);
	}

	return 0xFFFF;
}

static u16 next_free(u16 start)
{
	u32 i;
	for(i = start; i < BLOCKS && alloc_map[i].status != FREE; i++);
	return i;
}

static u16 find_fit(u16 blocks)
{
	u16 free_index = next_free(offset);
	u32 count = 0;
	while(free_index < BLOCKS)
	{

		if(count == blocks)
		{
			return free_index - blocks;
		}

		if(alloc_map[free_index].status == FREE)
		{
			free_index++;
			count++;
		}
		else
		{
			free_index = next_free(free_index);
			count = 0;
		}
	}

	return 0xFFFF;
}

u16 spr_vram_alloc(const char* ident, bool* status, u32 size)
{
	u32 blocks = PTR_ALIGN(size);
	u32 hash = fnv1a_string_hash(ident);

	debug_print("Ident: %s, Hash: 0x%.6X\n", ident, hash);
	u16 result = find_match(hash);
	
	if(result > BLOCKS)
	{
		debug_print("Miss! Size: %d, Blocks: %d\n", size, blocks);
		result = find_fit(blocks);
		if(result < BLOCKS)
		{
			ALLOC_ENTRY entry = { USED, hash, 1 };
			alloc_map[result] = entry;
			
			u32 i;
			for(i = result + 1; i < BLOCKS && i < result + blocks; i++)
			{
				alloc_map[i].status = CONTINUE;
			}
		}
		else
		{
			debug_print("OUT OF SPRITE VRAM!%s\n", "");
		}
	}
	else
	{
		alloc_map[result].ref_count++;
		*status = true;
	}

	debug_print("Allocated pointer: %p\n", base + result);
	return result;
}

void spr_vram_free(u16 ptr)
{
	if(ptr < 0 || ptr > BLOCKS || alloc_map[ptr].status != USED)
	{
		return;
	}

	debug_print("Freed pointer: %p\n", base + ptr);

	debug_print("ref_count == %d\n", alloc_map[ptr].ref_count);

	alloc_map[ptr].ref_count--;

	if(alloc_map[ptr].ref_count == 0)
	{
		debug_print("Released!%s", "\n");

		alloc_map[ptr].status = FREE;
		u32 i;
		for(i = ptr + 1; i < BLOCKS && alloc_map[i].status == CONTINUE; i++)
		{
			alloc_map[i].status = FREE;
		}
	}
}

u16 spr_vram_load(const char* ident, const void* src, u32 size)
{
	bool cached;
	u16 sprite = spr_vram_alloc(ident, &cached, size);
	if (!cached)
		CpuFastSet(src, spr_mem(sprite), size >> 2);
	return sprite;
}

/*typedef union Block
{
	struct
	{
		union Block* next;
		u32 size;
	} meta;

	TILE tile; // Force alginment to tiles
} Block;

Block* base = (Block*)&tile_mem[4][0];
Block* head = NULL;

u32 tile_index(void* ptr)
{
	void* start = base;
	u32 index = (u32)(ptr - start);
    index /= sizeof(TILE); 
    return index;
}

void* spr_vram_alloc(u32 size)
{
	if(head == NULL)
	{
		head = base;
		head->meta.next = NULL;
		head->meta.size = sizeof(TILE) * 1024;
	}

	u32 block_size = PTR_ALIGN(size);
	debug_print("Block size: %d\n", block_size);

	Block* current;
	Block* previous = head;
	for(current = head; current != NULL; current = current->meta.next)
	{
		if(current->meta.size >= block_size)
		{
			if(current->meta.size == block_size)
			{
				previous->meta.next = current->meta.next;
			}
			else
			{
				current->meta.size -= block_size;
				current += current->meta.size / sizeof(TILE); // Divide by 32
				debug_print("current: %p\n", current);
				current->meta.size = block_size;
			}

			head = previous;
			return current;
		}

		previous = current;
	}

	return NULL;
}

void spr_vram_free(void* ptr)
{
	Block* block = ptr;
	Block* current;
	for(current = head; 
		block <= current || block >= current->meta.next; 
		current = current->meta.next)
	{

	}
}*/