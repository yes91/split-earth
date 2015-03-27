#ifndef _Player_h_
#define _Player_h_

#include "Sprite.h"
#include "Vector2.h"

#include <gba_types.h>

struct GBFS_FILE;

typedef enum Direction
{
	WEST = 4, SOUTH = 5, EAST = 6, NORTH = 7
} Direction;

typedef struct Player 
{
	Sprite sprite;
	int z;
	Vector2 velocity;
	Vector2 forward;
	Direction heading;
} Player;

void Player_construct(
	Player* self,
	int x,
	int y,
	const struct GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* anims,
	int palette,
	int slot
	);

void Player_update(Player* self, FIXED dt);

void Player_draw(Player* self, FIXED offset_x, FIXED offset_y);

void Player_destroy(Player* self);

#endif