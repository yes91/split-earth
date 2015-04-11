#ifndef _hash_h_
#define _hash_h_

#include <gba_types.h>

#define FNV32_PRIME 16777619
#define FNV32_OFFSET_BASIS 2166136261U

u32 fnv1a_string_hash(const char* string);

u32 fnv1a_hash(const u8* data, u32 size);

#endif