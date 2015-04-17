#ifndef _Player_h_
#define _Player_h_

#include "Character.h"

struct GBFS_FILE;

typedef struct Player 
{
	Character base;
} Player;

void Player_construct(
	Player* self,
	Vector2 pos,
	const struct GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* sprite,
	int palette
	);

void Player_load(Player* self, Vector2 pos, const struct GBFS_FILE* dat, const char* player);

void Player_update(Player* self, FIXED dt);

void Player_draw(Player* self, FIXED offset_x, FIXED offset_y);

void Player_destroy(Player* self);

#endif
