#ifndef _filesys_h_
#define _filesys_h_

#include <stdlib.h>
#include <string.h>

static inline u32 read_data(void* dst, const void* src, u32 size)
{
	memcpy(dst, src, size);
	return size;
}

static inline u32 read_string(void* dst, const void* src, u32 size)
{
	memcpy(dst, src, size);
	char* string = dst;
	string[size] = '\0';
	return size;
}

#endif