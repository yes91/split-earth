#ifndef _PoolAllocator_h_
#define _PoolAllocator_h_

#include <gba_types.h>

typedef struct PoolAllocator
{
	void** head;
} PoolAllocator;

void PoolAllocator_construct(PoolAllocator* self, void* start, u32 size, u32 align);

void* PoolAllocator_alloc(PoolAllocator* self);

void PoolAllocator_free(PoolAllocator* self, void* ptr);

#endif