#include "PoolAllocator.h"

#include <gba_base.h>
#include "debug.h"

void PoolAllocator_construct(PoolAllocator* self, void* start, u32 size, u32 align)
{
	self->head = start;
	void** current = self->head;
	int i;
	for(i = 1; i < size; i++)
	{
		*current = start + i * align;
		current = *current;
	}

	*current = NULL;
}

void* PoolAllocator_alloc(PoolAllocator* self)
{   
	void* result = self->head;
	debug_print("Allocated pointer: %p\n", result);
	self->head = *self->head;
	return result;
}

void PoolAllocator_free(PoolAllocator* self, void* ptr)
{
	void** block = ptr;
	*block = self->head;
	self->head = block;
}