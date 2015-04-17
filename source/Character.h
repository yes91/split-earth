#ifndef _Character_h_
#define _Character_h_

#include "Sprite.h"
#include "Vector2.h"

#include <gba_types.h>

struct GBFS_FILE;

typedef enum Direction
{
	WEST = 4, SOUTH = 5, EAST = 6, NORTH = 7
} Direction;

typedef struct Character 
{
	Sprite sprite;
	int z;
	Vector2 velocity;
	Vector2 forward;
	Direction heading;
	u16 sprite_graphics;
} Character;

void Character_construct(
	Character* self,
	Vector2 pos,
	const struct GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* sprite,
	int palette
	);

void Character_decode(Character* self, Vector2 pos, const struct GBFS_FILE* dat, const u8* src);

void Character_draw(Character* self, FIXED offset_x, FIXED offset_y);

void Character_destroy(Character* self);

#endif