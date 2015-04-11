#include "hash.h"

u32 fnv1a_string_hash(const char* string)
{
	u32 hash = FNV32_OFFSET_BASIS;
	char cur = *string++;
	while(cur)
	{
		hash ^= cur;
		hash *= FNV32_PRIME;
		cur = *string++;
	}

	return hash;
}

u32 fnv1a_hash(const u8* data, u32 size)
{
	u32 hash = FNV32_OFFSET_BASIS;
	u32 i;
	for(i = 0; i < size; i++)
	{
		hash ^= data[i];
		hash *= FNV32_PRIME;
	}

	return hash;
}