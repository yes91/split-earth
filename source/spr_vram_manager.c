#include "spr_vram_manager.h"
#include <gba_sprites.h>
#include "tile.h"
#include "debug.h"

#define PTR_ALIGN(size) ((((size) + (sizeof(TILE)-1)) & ~(sizeof(TILE)-1)) / sizeof(TILE))
#define BLOCKS 1024

static TILE* base = &tile_mem[4][0];

typedef enum ALLOC_STATUS
{
	FREE,
	USED,
	CONTINUE
} ALLOC_STATUS;

static ALLOC_STATUS alloc_map[BLOCKS];

void spr_vram_init(void)
{
	u32 i;
	for(i = 0; i < BLOCKS; i++)
		alloc_map[i] = FREE;
}

void* spr_mem(u16 ptr)
{
	return base + ptr;
}

static u16 next_free(u16 start)
{
	u32 i;
	for(i = start; i < BLOCKS && alloc_map[i] != FREE; i++);
	return i;
}

static u16 find_fit(u16 blocks)
{
	u16 free_index = next_free(0);
	u32 count = 0;
	while(free_index < BLOCKS)
	{

		if(count == blocks)
		{
			return free_index - blocks;
		}

		if(alloc_map[free_index] == FREE)
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

	return -1;
}

u16 spr_vram_alloc(u32 size)
{
	u32 blocks = PTR_ALIGN(size);
	debug_print("Size: %d, Blocks: %d\n", size, blocks);
	u16 result = find_fit(blocks);
	if(result < BLOCKS)
	{
		alloc_map[result] = USED;
		u32 i;
		for(i = result + 1; i < BLOCKS && i < result + blocks; i++)
		{
			alloc_map[i] = CONTINUE;
		}
	}
	debug_print("Allocated pointer: %p\n", base + result);
	return result;
}

void spr_vram_free(u16 ptr)
{
	if(ptr < 0 || ptr > BLOCKS || alloc_map[ptr] != USED)
	{
		return;
	}

	alloc_map[ptr] = FREE;
	u32 i;
	for(i = ptr + 1; i < BLOCKS && alloc_map[i] == CONTINUE; i++)
	{
		alloc_map[i] = FREE;
	}
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